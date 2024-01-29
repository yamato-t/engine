#include "dx12/resource/texture.h"
#include "../file_loader/texture/WICTextureLoader12.h"
#include "../file_loader/texture/d3dx12.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	テクスチャビュー（SRV）を取得する
 * @return	ビュー（ハンドル）
 */
D3D12_CPU_DESCRIPTOR_HANDLE Texture::view() noexcept {
    return heap_->GetCPUDescriptorHandleForHeapStart();
}

//---------------------------------------------------------------------------------
/**
 * @brief	テクスチャをファイルから作成する
 * @param	path ファイルパス
 * @return	作成に成功した場合は true
 */
bool Texture::loadFromFile(std::string_view path, const dx12::CommandList& uploadCommandList) noexcept {
    D3D12_SUBRESOURCE_DATA subRes{};
    // ファイル読み込み
    {
        std::unique_ptr<uint8_t[]> decodedData{};
        auto                       temp = std::wstring(path.begin(), path.end());

        // D3D12_RESOURCE_STATE_COPY_DEST の CreateCommittedResource は LoadWICTextureFromFile 内で処理されている
        auto res = DirectX::LoadWICTextureFromFile(Device::instance().device(), temp.data(), resources_.GetAddressOf(), decodedData, subRes);
        if (FAILED(res)) {
            ASSERT(false, "テクスチャ読み込みと生成に失敗");
        }
        resources_->SetName(temp.data());
        resourcesDesc_ = resources_->GetDesc();
    }

    // GPU への転送
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> stagingTexture;

        auto   bufferSize = GetRequiredIntermediateSize(resources_.Get(), 0, 1);
        auto&& upload     = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto&& buffer     = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
        Device::instance().device()->CreateCommittedResource(&upload, D3D12_HEAP_FLAG_NONE, &buffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(stagingTexture.GetAddressOf()));
        UpdateSubresources(uploadCommandList.get(), resources_.Get(), stagingTexture.Get(), 0, 0, 1, &subRes);
        auto&& barrier = CD3DX12_RESOURCE_BARRIER::Transition(resources_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        uploadCommandList.get()->ResourceBarrier(1, &barrier);
    }

    // ビューの生成
    {
        if (!create()) {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	テクスチャビューを生成する
 * @return	成功した場合は true
 */
bool Texture::create() noexcept {
    // テクスチャービュー（SRV)のディスクリプターヒープの作成
    D3D12_DESCRIPTOR_HEAP_DESC shdesc = {};
    shdesc.NumDescriptors             = 1;
    shdesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    shdesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    auto res                          = Device::instance().device()->CreateDescriptorHeap(&shdesc, IID_PPV_ARGS(heap_.GetAddressOf()));
    if (FAILED(res)) {
        ASSERT(false, "テクスチャービュー（SRV)のディスクリプターヒープの作成に失敗");
        return false;
    }

    // ビュー（SRV)を作る
    D3D12_SHADER_RESOURCE_VIEW_DESC sdesc = {};
    sdesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    sdesc.Format                          = resourcesDesc_.Format;
    sdesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    sdesc.Texture2D.MipLevels             = resourcesDesc_.MipLevels;  // 1;
    Device::instance().device()->CreateShaderResourceView(resources_.Get(), &sdesc, heap_->GetCPUDescriptorHandleForHeapStart());

    return true;
}

}  // namespace dx12::resource
