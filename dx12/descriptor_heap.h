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
     * @brief	ヒープ種類
     */
    enum class Type {
        CBV_SRV_UAV = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        RTV         = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        DSV         = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
    };

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	登録情報
     */
    struct Handle {
        uint32_t                    index_{};
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};
        uint32_t                    incrementSize_{};
    };

    //---------------------------------------------------------------------------------
    /**
     * @brief	解放予定のディスクリプタ情報
     */
    struct PendingFree {
        uint32_t index_{};
        uint8_t  waitFrame_{};
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
     * @return	作成に成功した場合は true
     */
    bool create(Type type, uint32_t capacity) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	解放予定のディスクリプタを解放する
     */
    void applyFree() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタを確保する
     * @return	CPU と GPU のディスクリプタハンドル
     */
    [[nodiscard]] Handle allocate() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ヒープから指定数を確保する
     * @param	handle 解放するディスクリプタのハンドル
     */
    void free(const Handle& handle) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタを指定数を確保する
     * @param	num			確保数
     * @return	CPU と GPU のディスクリプタハンドルリスト
     */
    [[nodiscard]] std::vector<Handle> allocate(uint32_t num) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ヒープをコマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     */
    void setToCommandList(dx12::CommandList& commandList) noexcept;

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスを指定してハンドルを取得する
     * @param	index		インデックス
     * @return	CPU と GPU のディスクリプタハンドル
     */
    [[nodiscard]] Handle handleFromIndex(uint32_t index) noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_{};      ///< ディスクリプタヒープ
    uint32_t                                     capacity_{};  ///< 最大管理数
    D3D12_DESCRIPTOR_HEAP_DESC                   desc_{};      ///< ディスクリプタヒープフォーマット情報

    std::vector<uint32_t>    freeIndex_{};    ///< 空いているインデックス
    std::vector<PendingFree> pendingFree_{};  ///< 解放予定のディスクリプタ情報
};
}  // namespace dx12
