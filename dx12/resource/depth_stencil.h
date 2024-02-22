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
     * @brief	デプスステンシルを作成する
     * @return	作成に成功した場合は true
     */
    bool create() noexcept;
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
     * @brief	デストラクタ
     */
    ~DepthStencil() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デプスステンシルを作成する
     * @return	作成に成功した場合は true
     */
    [[nodiscard]] bool create() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ビューを生成する
     * @param	descriptorHeap	ビュー（ディスクリプタ）登録先のヒープ
     */
    void createView(DescriptorHeap& descriptorHeap) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デプスステンシルビューを取得する
     * @return	デプスステンシルビュー（ハンドル）
     */
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE view() noexcept;

private:
    DescriptorHeap                        heap_{};      ///< ディスクリプタヒープ
    DescriptorHeap::Handle                handle_{};    ///< ヒープ登録ハンドル
    std::unique_ptr<DepthStencilResource> resource_{};  ///< リソース
};
}  // namespace dx12::resource
