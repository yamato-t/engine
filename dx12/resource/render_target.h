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
     * @brief	コンストラクタ
     */
    RenderTarget() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~RenderTarget() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットを生成する
	 * @param	w h			横と縦のサイズ
	 * @param	num			ターゲット数
	 * @param	format		フォーマット
     * @return	成功した場合は true
     */
    bool create(uint32_t w, uint32_t h, uint32_t num, DXGI_FORMAT format) noexcept;

	//---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットを開始する
     * @param	commandList	利用するコマンドリスト
     */
    void startRendering(CommandList& commandList) noexcept;

	//---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットを終了する
     * @param	commandList	利用するコマンドリスト
     */
    void finishRendering(CommandList& commandList) noexcept;

public:
    dx12::DescriptorHeap      descriptorHeap_{};  ///< ディスクリプタヒープ
    resource::TextureResource texture_{};         ///< テクスチャ
    resource::DepthStencil    depthStencil_{};    ///< デプスステンシル
};
}  // namespace dx12::resource
