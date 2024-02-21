#include "dx12/resource/depth_stencil.h"

#include "window/window.h"

namespace dx12::resource {


//---------------------------------------------------------------------------------
/**
 * @brief	デプスステンシルを作成する
 * @return	作成に成功した場合は true
 */
bool DepthStencilResource::create() noexcept {

    if (!createCommittedResource(window::width(), window::height())) {
        return false;
	}
    setName("デプスステンシル");


	// デスクリプタヒープを作成する
	descriptorHeap_.create(3, 1, 0);
	handle_ = descriptorHeap_.registerBuffer(1);

	// ディスクリプタを作成する
	D3D12_CPU_DESCRIPTOR_HANDLE handle{};
    handle.ptr = handle_.cpuHandle_;
    dx12::Device::instance().device()->CreateDepthStencilView(gpuResource_.Get(), nullptr, handle);

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	デプスステンシルビューを取得する
 * @return	デプスステンシルビュー（ハンドル）
 */
D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilResource::view() noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE handle{};
    handle.ptr = handle_.cpuHandle_;

	return handle;
}

//---------------------------------------------------------------------------------
/**
 * @brief	GPUリソースを作成する
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool DepthStencilResource::createCommittedResource(uint32_t stride, uint32_t num) noexcept {

	D3D12_HEAP_PROPERTIES heapProperty{};
    heapProperty.Type                 = D3D12_HEAP_TYPE_DEFAULT;
    heapProperty.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperty.CreationNodeMask     = 1;
    heapProperty.VisibleNodeMask      = 1;

    D3D12_CLEAR_VALUE dsvClearValue{};
    dsvClearValue.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvClearValue.DepthStencil.Depth   = 1.0f;
    dsvClearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Alignment          = 0;
    resourceDesc.Width              = stride;
    resourceDesc.Height             = num;
    resourceDesc.DepthOrArraySize   = 1;
    resourceDesc.MipLevels          = 1;
    resourceDesc.Format             = dsvClearValue.Format;
    resourceDesc.SampleDesc.Count   = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;


    auto res = dx12::Device::instance().device()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE,
                                                                          &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                                          &dsvClearValue, IID_PPV_ARGS(gpuResource_.GetAddressOf()));
    if (FAILED(res)) {
        ASSERT(false, "デプスステンシルバッファの作成に失敗");
        return false;
    }

    resourcesDesc_ = gpuResource_->GetDesc();

    alignedStride_ = stride;
    num_           = num;
    size_          = num_ * alignedStride_;

    return true;
}

}  // namespace dx12::resource
