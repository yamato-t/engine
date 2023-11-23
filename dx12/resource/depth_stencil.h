#pragma once

#include "dx12/command_list.h"
#include "utility/noncopyable.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * デプスステンシル
 */
class DepthStencil final : utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    DepthStencil() = default;

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

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_{};                ///< ディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12Resource>       resources_{};           ///< リソース
};
}  // namespace dx12::resource
