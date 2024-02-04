#pragma once

#include "dx12/command_list.h"
#include "dx12/resource/depth_stencil.h"
#include "utility/noncopyable.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * レンダーターゲット
 */
class RenderTarget final : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     * @param	bufferNum	レンダーターゲット数
     */
    RenderTarget(uint32_t bufferNum);

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~RenderTarget() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	バックバッファ用のレンダーターゲットビューを作成する
     * @param	swapChain	バッファ本体を持っているスワップチェイン
     * @return	作成に成功した場合は true
     */
    [[nodiscard]] bool createView(IDXGISwapChain3* swapChain) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットへのレンダリングを開始する
     * @param	commandList	利用するコマンドリスト
     */
    void startRendering(CommandList& commandList) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットをレンダーターゲットに設定する
     * @param	commandList	利用するコマンドリスト
     */
    void setToRenderTarget(CommandList& commandList) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットへのレンダリングを終了する
     * @param	commandList	利用するコマンドリスト
     */
    void finishRendering(CommandList& commandList) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットのインデックスを更新する
     * @return	新しいインデックス
     */
    void updateBufferIndex(uint32_t index) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットのバッファ数を取得する
     * @return	レンダーターゲットのバッファ数
     */
    uint32_t bufferNum() const noexcept;

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットビューを取得する
     * @return	レンダーターゲットビュー（ハンドル）
     */
    D3D12_CPU_DESCRIPTOR_HANDLE view() const noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        heap_{};                ///< ディスクリプタヒープ
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> resources_{};           ///< リソース
    const uint32_t                                      bufferNum_{};           ///< バッファ数
    uint32_t                                            size_{};                ///< ディスクリプタサイズ
    uint32_t                                            currentBufferIndex_{};  ///< 現在のバッファインデックス
    resource::DepthStencil                              depthStencil_{};         // デプスステンシル
};
}  // namespace dx12::resource
