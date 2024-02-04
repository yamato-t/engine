#include "dx12/resource/texture.h"
#include "dx12/command_queue.h"
#include "dx12/fence.h"
#include "../file_loader/texture/WICTextureLoader12.h"
#include "../file_loader/texture/d3dx12.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	コンストラクタ
 */
Texture::Texture(Texture&& src) noexcept
    : GpuResource(std::move(src)) {
    resourcesDesc_ = src.resourcesDesc_;
    handle_        = src.handle_;

    src.resourcesDesc_ = {};
    src.handle_        = {};
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

    // GPUリソースの作成とファイルの読み込み
    {
        auto temp = std::wstring(path.begin(), path.end());

        // D3D12_RESOURCE_STATE_COPY_DEST の CreateCommittedResource は LoadWICTextureFromFile 内で処理されている
        auto res = DirectX::LoadWICTextureFromFile(Device::instance().device(), temp.data(),
                                                   gpuResource_.GetAddressOf(), decodedData, subRes);
        if (FAILED(res)) {
            ASSERT(false, "テクスチャ読み込みと生成に失敗");
        }

        resourcesDesc_ = gpuResource_->GetDesc();

        // テクスチャデータのサイズ情報だけ設定する;
        createCommittedResource(subRes.RowPitch, resourcesDesc_.Height);
    }

    // 上記で作成したGPUメモリ上のリソース（GPUリソース）へ読み込んだデータを転送する
    {
        CommandList  uploadCommandList{};
        CommandQueue commandQueue{};
        Fence        fence{};

        uploadCommandList.create();
        commandQueue.create();
        fence.create();

        Microsoft::WRL::ComPtr<ID3D12Resource> stagingTexture;

        auto   bufferSize = GetRequiredIntermediateSize(gpuResource_.Get(), 0, 1);
        auto&& upload     = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto&& buffer     = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
        Device::instance().device()->CreateCommittedResource(
            &upload, D3D12_HEAP_FLAG_NONE,
            &buffer, D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr, IID_PPV_ARGS(stagingTexture.GetAddressOf()));

        uploadCommandList.reset();
        UpdateSubresources(uploadCommandList.get(), gpuResource_.Get(), stagingTexture.Get(), 0, 0, 1, &subRes);
        auto&& barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            gpuResource_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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

    setName(path.data());

    return true;
}

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
    sdesc.Texture2D.MipLevels             = resourcesDesc_.MipLevels;

    D3D12_CPU_DESCRIPTOR_HANDLE handle{};
    handle.ptr = handle_.cpuHandle_;
    Device::instance().device()->CreateShaderResourceView(gpuResource_.Get(), &sdesc, handle);
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
 * @brief	GPUリソースを作成する
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool Texture::createCommittedResource(uint32_t stride, uint32_t num) noexcept {
    alignedStride_ = stride;
    num_           = num;
    size_          = num_ * alignedStride_;

    return true;
}

}  // namespace dx12::resource
