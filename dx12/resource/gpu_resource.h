#pragma once

#include "dx12/device.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"
#include "utility/noncopyable.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * GPU リソース
 */
class GpuResource : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    GpuResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    virtual ~GpuResource() = default;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	バッファを生成する
     * @param	data		データの先頭アドレス
     * @param	stride		バッファのストライド
     * @param	num			バッファの数
     * @return	作成に成功した場合は true
     */
    [[nodiscard]] virtual bool create(void* data, uint32_t stride, uint32_t num) noexcept = 0;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープに登録する
     * @param	descriptorHeap	登録先のヒープ
     */
    virtual void registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept = 0;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	rootParameterIndex		ルートパラメータのインデックス
     * @param	index					バッファのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex, uint32_t index = 0) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	オフセットを取得する
     * @param	index			バッファのインデックス
     * @return	インデックスに対応するオフセット
     */
    [[nodiscard]] uint64_t offset(uint32_t index) const noexcept;

protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource_{};  ///< リソース
    uint32_t                               alignedSize_{};  ///< バッファサイズ（アラインメント済み）
    uint32_t                               num_{};          ///< バッファ数
    DescriptorHeap::RegisterHandle         handle_{};       ///< ヒープ登録ハンドル
};

}  // namespace dx12::resource
