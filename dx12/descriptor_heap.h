#pragma once

#include "dx12/device.h"
#include "dx12/command_list.h"

#include "utility/noncopyable.h"

namespace dx12 {

//---------------------------------------------------------------------------------
/**
 * @brief
 * ディスクリプタヒープ
 */
class DescriptorHeap final : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	登録情報
     */
    struct RegisterHandle {
        uint32_t index_{};
        uint64_t cpuHandle_{};
        uint64_t gpuHandle_{};
        uint32_t incrementSize_{};
    };

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    DescriptorHeap() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~DescriptorHeap() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープを生成する
     * @param	type			ヒープが管理する対象の種類
     * @param	capacity		最大管理数
     * @param	flags			フラグ設定
     * @return	作成に成功した場合は true
     */
    bool create(uint32_t type, uint32_t capacity, uint32_t flags) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ヒープにバッファを登録する
     * @param	num			登録数
     * @return	CPU と GPU のディスクリプタハンドル
     */
    [[nodiscard]] RegisterHandle registerBuffer(uint32_t num) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ヒープをコマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     */
    void setToCommandList(dx12::CommandList& commandList) noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_{};          ///< ディスクリプタヒープ
    uint32_t                                     currentIndex_{};  ///< 現在の登録番号
    uint32_t                                     capacity_{};      ///< 最大管理数
    D3D12_DESCRIPTOR_HEAP_DESC                   desc_{};		   ///< ディスクリプタヒープフォーマット情報
};
}  // namespace dx12
