#pragma once

#include "dx12/command_list.h"
#include "utility/noncopyable.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * テクスチャ
 */
class Texture final : utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    Texture() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~Texture() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャビュー（SRV）を取得する
     * @return	ビュー（ハンドル）
     */
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE view() noexcept;

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
     * @brief	テクスチャをファイルから読み込む
     * @param	path				ファイルパス
     * @return	成功した場合は true
     */
    [[nodiscard]] bool loadFromFile(std::string_view path) noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource>       resources_{};      ///< リソース
    D3D12_RESOURCE_DESC                          resourcesDesc_{};  ///< リソースフォーマット情報
    DescriptorHeap::RegisterHandle               handle_{};         ///< ヒープ登録ハンドル
};
}  // namespace dx12::resource
