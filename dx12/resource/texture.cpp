#include "dx12/resource/texture.h"
#include "dx12/command_queue.h"
#include "dx12/fence.h"
#include "../file_loader/texture/WICTextureLoader12.h"
#include "../file_loader/texture/d3dx12.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープに登録する
 * @param	descriptorHeap			登録先のヒープ
 */
void Texture::registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept {
    // ヒープ登録ハンドルを取得する
    handle_ = descriptorHeap.registerBuffer(1);

    D3D12_SHADER_RESOURCE_VIEW_DESC sdesc = {};
    sdesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    sdesc.Format                          = resourcesDesc_.Format;
    sdesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    sdesc.Texture2D.MipLevels             = resourcesDesc_.MipLevels;  // 1;

    D3D12_CPU_DESCRIPTOR_HANDLE handle{};
    handle.ptr = handle_.cpuHandle_;
    Device::instance().device()->CreateShaderResourceView(resources_.Get(), &sdesc, handle);
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList				設定先のコマンドリスト
 * @param	rootParameterIndex		ルートパラメータのインデックス
 */
void Texture::setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept {
    D3D12_GPU_DESCRIPTOR_HANDLE handle{};
    handle.ptr = handle_.gpuHandle_;
    commandList.get()->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
}

//---------------------------------------------------------------------------------
/**
 * @brief	テクスチャをファイルから作成する
 * @param	path ファイルパス
 * @return	作成に成功した場合は true
 */
bool Texture::create(std::string_view path) noexcept {
    D3D12_SUBRESOURCE_DATA     subRes{};
    std::unique_ptr<uint8_t[]> decodedData{};

    // ファイル読み込み
    {
        auto temp = std::wstring(path.begin(), path.end());

        // D3D12_RESOURCE_STATE_COPY_DEST の CreateCommittedResource は LoadWICTextureFromFile 内で処理されている
        auto res = DirectX::LoadWICTextureFromFile(Device::instance().device(), temp.data(),
                                                   resources_.GetAddressOf(), decodedData, subRes);
        if (FAILED(res)) {
            ASSERT(false, "テクスチャ読み込みと生成に失敗");
        }
        resources_->SetName(temp.data());
        resourcesDesc_ = resources_->GetDesc();
    }

    // GPU への転送
    {
        CommandList  uploadCommandList{};
        CommandQueue commandQueue{};
        Fence        fence{};

        uploadCommandList.create();
        commandQueue.create();
        fence.create();

        Microsoft::WRL::ComPtr<ID3D12Resource> stagingTexture;

        auto   bufferSize = GetRequiredIntermediateSize(resources_.Get(), 0, 1);
        auto&& upload     = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto&& buffer     = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
        Device::instance().device()->CreateCommittedResource(&upload, D3D12_HEAP_FLAG_NONE, &buffer,
                                                             D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                             IID_PPV_ARGS(stagingTexture.GetAddressOf()));

        uploadCommandList.reset();
        UpdateSubresources(uploadCommandList.get(), resources_.Get(), stagingTexture.Get(), 0, 0, 1, &subRes);
        auto&& barrier = CD3DX12_RESOURCE_BARRIER::Transition(resources_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        uploadCommandList.get()->ResourceBarrier(1, &barrier);
        uploadCommandList.get()->Close();
        std::array<ID3D12CommandList*, 1> lists = {uploadCommandList.get()};
        commandQueue.get()->ExecuteCommandLists(1, static_cast<ID3D12CommandList**>(lists.data()));

        fence.get()->Signal(0);
        commandQueue.get()->Signal(fence.get(), 1);

        auto event = CreateEvent(nullptr, false, false, "WAIT_GPU");
        fence.get()->SetEventOnCompletion(1, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }

    return true;
}


}  // namespace dx12::resource
