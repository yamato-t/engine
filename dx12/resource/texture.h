#pragma once

#include "dx12/command_list.h"
#include "utility/noncopyable.h"
#include "dx12/command_list.h"

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
     * @brief	テクスチャをファイルから読み込む
     * @param	path				ファイルパス
	 * @param	uploadCommandList	GPU転送用コマンドリスト
	 * @return	成功した場合は true
     */
    [[nodiscard]] bool loadFromFile(std::string_view path, const dx12::CommandList& uploadCommandList) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャビューを生成する
     * @return	成功した場合は true
     */
    [[nodiscard]] bool create() noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_{};       ///< ディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12Resource>       resources_{};  ///< リソース
    D3D12_RESOURCE_DESC                          resourcesDesc_{};
};
}  // namespace dx12::resource
