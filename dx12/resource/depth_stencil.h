#pragma once

#include "dx12/command_list.h"
#include "utility/noncopyable.h"
#include "dx12/resource/gpu_resource.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * デプスステンシルリソース
 */
class DepthStencilResource final : public GpuResource {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    DepthStencilResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~DepthStencilResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャをファイルから読み込む
     * @param	path				ファイルパス
     * @return	成功した場合は true
     */
    [[nodiscard]] bool create() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デプスステンシルビューを取得する
     * @return	デプスステンシルビュー（ハンドル）
     */
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE view() noexcept;

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	GPUリソースを作成する
     * @param	stride		バッファのストライド
     * @param	num			バッファの数
     * @return	作成に成功した場合は true
     */
    [[nodiscard]] virtual bool createCommittedResource(uint32_t stride, uint32_t num) noexcept override;

private:
    D3D12_RESOURCE_DESC            resourcesDesc_{};  ///< リソースフォーマット情報
    dx12::DescriptorHeap           descriptorHeap_{};  ///< ディスクリプタヒープ
    DescriptorHeap::RegisterHandle handle_{};         ///< ヒープ登録ハンドル
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * デプスステンシル
 */
class DepthStencil final : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    DepthStencil() { resource_ = std::make_unique<DepthStencilResource>(); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ムーブコンストラクタ
     */
    DepthStencil(DepthStencil&& src) { resource_ = std::move(src.resource_); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~DepthStencil() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デプスステンシルを作成する
     * @return	作成に成功した場合は true
     */
    bool create() noexcept {
        return resource_->create();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	デプスステンシルビューを取得する
     * @return	デプスステンシルビュー（ハンドル）
     */
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE view() noexcept {
        return resource_->view();
    }

private:
    std::unique_ptr<DepthStencilResource> resource_{};
};
}  // namespace dx12::resource
