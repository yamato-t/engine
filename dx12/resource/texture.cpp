#include "dx12/resource/texture.h"
#include "dx12/command_queue.h"
#include "dx12/fence.h"
#include "../file_loader/texture/WICTextureLoader12.h"
#include "../file_loader/texture/d3dx12.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	テクスチャをファイルから作成する
 * @param	path ファイルパス
 * @return	作成に成功した場合は true
 */
bool TextureResource::create(std::string_view path) noexcept {
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
 * @brief	テクスチャを引数から作成する
 * @param	w h			横と縦のサイズ
 * @param	mipLevel	ミップマップレベル
 * @param	arraySize	テクスチャ配列サイズ
 * @param	format		テクスチャフォーマット
 * @return	成功した場合は true
 */
bool TextureResource::create(uint32_t w, uint32_t h, uint32_t mipLevel, uint32_t arraySize, DXGI_FORMAT format) noexcept {
    // GPU リソースの作成
    CD3DX12_RESOURCE_DESC desc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        w,
        h,
        arraySize,
        mipLevel,
        format,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    auto&& prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format   = desc.Format;
    clearValue.Color[0] = 1.0f;
    clearValue.Color[1] = 0.0f;
    clearValue.Color[2] = 0.0f;
    clearValue.Color[3] = 1.0f;

    Device::instance().device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        &clearValue,
        IID_PPV_ARGS(gpuResource_.GetAddressOf()));

    resourcesDesc_ = gpuResource_->GetDesc();

    createCommittedResource(w, h);

    setName("テクスチャ");

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープに登録する
 * @param	descriptorHeap			登録先のヒープ
 */
void TextureResource::registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept {
    // ヒープ登録ハンドルを取得する
    handle_ = descriptorHeap.registerBuffer(1);
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList				設定先のコマンドリスト
 * @param	rootParameterIndex		ルートパラメータのインデックス
 */
void TextureResource::setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept {
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
bool TextureResource::createCommittedResource(uint32_t stride, uint32_t num) noexcept {
    alignedStride_ = stride;
    num_           = num;
    size_          = num_ * alignedStride_;

    return true;
}

}  // namespace dx12::resource
