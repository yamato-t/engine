#pragma once

#include "dx12/command_list.h"
#include "dx12/resource/depth_stencil.h"
#include "dx12/resource/texture.h"
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
     * @brief    コンストラクタ
     */
    RenderTarget() { resource_ = std::make_unique<TextureResource>(); }

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~RenderTarget() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    レンダーターゲットを生成する
     * @param    w h            横と縦のサイズ
     * @param    num            ターゲット数
     * @param    format        フォーマット
     * @return    成功した場合は true
     */
    bool create(uint32_t w, uint32_t h, uint32_t num, DXGI_FORMAT format) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ビューを生成する
     * @param	descriptorHeap	ビュー（ディスクリプタ）登録先のヒープ
     */
    void createView(DescriptorHeap& descriptorHeap) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	rootParameterIndex		ルートパラメータのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットへのレンダリングを開始する
     * @param    commandList    利用するコマンドリスト
     */
    void startRendering(CommandList& commandList) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットへのレンダリングを終了する
     * @param    commandList    利用するコマンドリスト
     */
    void finishRendering(CommandList& commandList) noexcept;

public:
    std::unique_ptr<TextureResource> resource_{};  ///< リソース
    DescriptorHeap::Handle           handle_{};    ///< ヒープ登録ハンドル

    dx12::DescriptorHeap   rtvDescriptorHeap_{};  ///< RTV用ディスクリプタヒープ
    DescriptorHeap::Handle rtvHandle_{};          ///< RTV用ヒープ登録ハンドル
    resource::DepthStencil depthStencil_{};       ///< デプスステンシル
};
}  // namespace dx12::resource