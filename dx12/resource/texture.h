#pragma once

#include "dx12/command_list.h"
#include "utility/noncopyable.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"
#include "dx12/resource/gpu_resource.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * テクスチャリソース
 */
class TextureResource final : public GpuResource {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    TextureResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~TextureResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャをファイルから読み込む
     * @param	path				ファイルパス
     * @return	成功した場合は true
     */
    bool create(std::string_view path) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャを引数から作成する
     * @param	w h			横と縦のサイズ
     * @param	mipLevel	ミップマップレベル
     * @param	arraySize	テクスチャ配列サイズ
	 * @param	format		テクスチャフォーマット
     * @return	成功した場合は true
     */
    bool create(uint32_t w, uint32_t h, uint32_t mipLevel, uint32_t arraySize, DXGI_FORMAT format) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープに登録する
     * @param	descriptorHeap			登録先のヒープ
     */
    void registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	rootParameterIndex		ルートパラメータのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	リソース設定を取得する
     */
    const D3D12_RESOURCE_DESC& resourcesDesc() const noexcept { return resourcesDesc_; }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープ登録ハンドルを取得する
     */
    const DescriptorHeap::RegisterHandle& handle() const noexcept { return handle_; }

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	GPUリソースを作成する
     * @param	stride		バッファのストライド
     * @param	num			バッファの数
     * @return	作成に成功した場合は true
     */
    bool createCommittedResource(uint32_t stride, uint32_t num) noexcept override;

public:
    D3D12_RESOURCE_DESC            resourcesDesc_{};  ///< リソースフォーマット情報
    DescriptorHeap::RegisterHandle handle_{};         ///< ヒープ登録ハンドル
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * テクスチャ
 */
class Texture final : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    Texture() { resource_ = std::make_unique<TextureResource>(); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ムーブコンストラクタ
     */
    Texture(Texture&& src) { resource_ = std::move(src.resource_); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~Texture() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャをファイルから読み込む
     * @param	path				ファイルパス
     * @return	成功した場合は true
     */
    [[nodiscard]] bool create(std::string_view path) noexcept {
        return resource_->create(path);
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
    [[nodiscard]] bool create(uint32_t w, uint32_t h, uint32_t mipLevel, uint32_t arraySize, DXGI_FORMAT format) noexcept {
        return resource_->create(w, h, mipLevel, arraySize, format);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープに登録する
     * @param	descriptorHeap			登録先のヒープ
     */
    void registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept {
        resource_->registerToDescriptorHeap(descriptorHeap);

        D3D12_SHADER_RESOURCE_VIEW_DESC sdesc = {};
        sdesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        sdesc.Format                          = resource_->resourcesDesc().Format;
        sdesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
        sdesc.Texture2D.MipLevels             = resource_->resourcesDesc().MipLevels;

        D3D12_CPU_DESCRIPTOR_HANDLE handle{};
        handle.ptr = resource_->handle().cpuHandle_;
        Device::instance().device()->CreateShaderResourceView(resource_->get(), &sdesc, handle);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	rootParameterIndex		ルートパラメータのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept {
        resource_->setToCommandList(commandList, rootParameterIndex);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     */
    void resetResource(TextureResource& tex, DescriptorHeap& descriptorHeap) noexcept {
        resource_->gpuResource_.Reset();
        resource_->gpuResource_   = tex.gpuResource_.Get();
        resource_->num_           = tex.num_;
        resource_->alignedStride_ = tex.alignedStride_;
        resource_->size_          = tex.size_;
        resource_->resourcesDesc_ = tex.resourcesDesc_;

        resource_->handle_ = descriptorHeap.get(resource_->handle_.index_);

		D3D12_SHADER_RESOURCE_VIEW_DESC sdesc = {};
        sdesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        sdesc.Format                          = resource_->resourcesDesc().Format;
        sdesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
        sdesc.Texture2D.MipLevels             = resource_->resourcesDesc().MipLevels;

        D3D12_CPU_DESCRIPTOR_HANDLE handle{};
        handle.ptr = resource_->handle().cpuHandle_;
        Device::instance().device()->CreateShaderResourceView(resource_->get(), &sdesc, handle);
    }

private:
    std::unique_ptr<TextureResource> resource_{};
};

}  // namespace dx12::resource
