#include "dx12/resource/render_target.h"

#include "window/window.h"

namespace dx12::resource {

namespace {
constexpr float defaultClearColor[4] = {0.0f, 0.5f, 0.0f, 1.0f};  ///< 画面をクリアする際の色
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
    // テクスチャリソースの作成
    if (!resource_->create(w, h, 1, 1, format, defaultClearColor)) {
        ASSERT(false, "RTV テクスチャリソースの作成に失敗しました");
        return false;
    }

    // RTV デスクリプタヒープを作成
    rtvDescriptorHeap_.create(dx12::DescriptorHeap::Type::RTV, num);
    rtvHandle_ = rtvDescriptorHeap_.allocate(num);
    // RTV ディスクリプタ作成
    Device::instance().device()->CreateRenderTargetView(resource_->get(), nullptr, rtvHandle_.cpuHandle_);

    // デプスステンシル作成
    if (!depthStencil_.create()) {
        ASSERT(false, "デプスステンシルの作成に失敗しました");
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	レンダーターゲットへのレンダリングを開始する
 * @param	commandList	利用するコマンドリスト
 */
void RenderTarget::startRendering(CommandList& commandList) noexcept {
    // レンダーターゲットとして利用する
    resourceBarrier(commandList, resource_->get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // レンダーターゲット
    D3D12_CPU_DESCRIPTOR_HANDLE handles[]            = {rtvHandle_.cpuHandle_};
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle[] = {depthStencil_.view()};
    commandList.get()->OMSetRenderTargets(1, handles, false, depthStencilHandle);

    // レンダーターゲットクリア
    commandList.get()->ClearRenderTargetView(rtvHandle_.cpuHandle_, defaultClearColor, 0, nullptr);
    commandList.get()->ClearDepthStencilView(depthStencil_.view(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // ビューポート
    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX       = 0.0f;
    viewport.TopLeftY       = 0.0f;
    viewport.Width          = static_cast<float>(resource_->stride());
    viewport.Height         = static_cast<float>(resource_->num());
    viewport.MinDepth       = D3D12_MIN_DEPTH;
    viewport.MaxDepth       = D3D12_MAX_DEPTH;
    commandList.get()->RSSetViewports(1, &viewport);

    // シザー矩形
    D3D12_RECT rect = {};
    rect.left       = 0;
    rect.top        = 0;
    rect.right      = resource_->stride();
    rect.bottom     = resource_->num();
    commandList.get()->RSSetScissorRects(1, &rect);
}

//---------------------------------------------------------------------------------
/**
 * @brief	レンダーターゲットへのレンダリングを終了する
 * @param	commandList	利用するコマンドリスト
 */
void RenderTarget::finishRendering(CommandList& commandList) noexcept {
    resourceBarrier(commandList, resource_->get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

//---------------------------------------------------------------------------------
/**
 * @brief	ビューを生成する
 * @param	descriptorHeap	ビュー（ディスクリプタ）登録先のヒープ
 */
void RenderTarget::createView(DescriptorHeap& descriptorHeap) noexcept {
    handle_ = descriptorHeap.allocate(1);

    D3D12_SHADER_RESOURCE_VIEW_DESC sdesc = {};
    sdesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    sdesc.Format                          = resource_->desc().Format;
    sdesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    sdesc.Texture2D.MipLevels             = resource_->desc().MipLevels;

    Device::instance().device()->CreateShaderResourceView(resource_->get(), &sdesc, handle_.cpuHandle_);
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList				設定先のコマンドリスト
 * @param	args					コマンドリスト設定時の引数
 */
void RenderTarget::setToCommandList(CommandList& commandList, const Args& agrs) noexcept {
    commandList.get()->SetGraphicsRootDescriptorTable(agrs.rootParameterIndex_, handle_.gpuHandle_);
}

}  // namespace dx12::resource
