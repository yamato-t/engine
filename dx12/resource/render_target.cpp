#include "dx12/resource/render_target.h"

#include "window/window.h"

namespace dx12::resource {

namespace {
constexpr float defaultClearColor[] = {1.0f, 1.0f, 0.0f, 1.0f};  ///< 画面をクリアする際の色
}

namespace {
//---------------------------------------------------------------------------------
/**
 * @brief	リソースにバリアを設定する
 * @param	commandList	コマンドリスト
 * @param	resource	バリアを張るリソース
 * @param	from		変更前のリソースステート
 * @param	to			変更後のリソースステート
 */
void resourceBarrier(CommandList& commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) noexcept {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource   = resource;
    barrier.Transition.StateBefore = from;
    barrier.Transition.StateAfter  = to;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList.get()->ResourceBarrier(1, &barrier);
}
}  // namespace

//---------------------------------------------------------------------------------
/**
 * @brief	レンダーターゲットを生成する
 * @param	w h			横と縦のサイズ
 * @param	num			ターゲット数
 * @param	format		フォーマット
 * @return	成功した場合は true
 */
bool RenderTarget::create(uint32_t w, uint32_t h, uint32_t num, DXGI_FORMAT format) noexcept {
    //// デスクリプタヒープを作成
    //descriptorHeap_.create(2, num, 0);
	//
    //// テクスチャ作成とヒープへの登録
    //if (!texture_.create(window::width(), window::height(), 1, 1, format)) {
    //    return false;
    //}
    //texture_.registerToDescriptorHeap(descriptorHeap_);
    //D3D12_CPU_DESCRIPTOR_HANDLE handle{};
    //handle.ptr = texture_.handle().cpuHandle_;
    //Device::instance().device()->CreateRenderTargetView(texture_.get(), nullptr, handle);
	//
    //// デプスステンシル作成
    //depthStencil_.create();

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	レンダーターゲットへのレンダリングを開始する
 * @param	commandList	利用するコマンドリスト
 */
void RenderTarget::startRendering(CommandList& commandList) noexcept {
    //resourceBarrier(commandList, texture_.get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//
    //D3D12_CPU_DESCRIPTOR_HANDLE handle{};
    //handle.ptr = texture_.handle().cpuHandle_;
	//
    //// レンダーターゲット
    //D3D12_CPU_DESCRIPTOR_HANDLE handles[]            = {handle};
    //D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle[] = {depthStencil_.view()};
    //commandList.get()->OMSetRenderTargets(1, handles, false, depthStencilHandle);
	//
    //// レンダーターゲットクリア
    //commandList.get()->ClearRenderTargetView(handle, defaultClearColor, 0, nullptr);
    //commandList.get()->ClearDepthStencilView(depthStencil_.view(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	//
    //// ビューポート
    //D3D12_VIEWPORT viewport = {};
    //viewport.TopLeftX       = 0.0f;
    //viewport.TopLeftY       = 0.0f;
    //viewport.Width          = static_cast<float>(window::width());
    //viewport.Height         = static_cast<float>(window::height());
    //viewport.MinDepth       = D3D12_MIN_DEPTH;
    //viewport.MaxDepth       = D3D12_MAX_DEPTH;
    //commandList.get()->RSSetViewports(1, &viewport);
	//
    //// シザー矩形
    //D3D12_RECT rect = {};
    //rect.left       = 0;
    //rect.top        = 0;
    //rect.right      = window::width();
    //rect.bottom     = window::height();
    //commandList.get()->RSSetScissorRects(1, &rect);
}

//---------------------------------------------------------------------------------
/**
 * @brief	レンダーターゲットへのレンダリングを終了する
 * @param	commandList	利用するコマンドリスト
 */
void RenderTarget::finishRendering(CommandList& commandList) noexcept {
    //resourceBarrier(commandList, texture_.get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

}  // namespace dx12::resource
