#include "dx12/resource/constant_buffer.h"

namespace {

//---------------------------------------------------------------------------------
/** @def
 * アラインメント
 */
#define ALIGN(size) ((size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1))
}  // namespace

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	コンスタントバッファを生成する
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool ConstantBufferResource::create(uint32_t stride, uint32_t num) noexcept {

    // GPUリソース作成
    D3D12_HEAP_PROPERTIES heapProperty = {};
    heapProperty.Type                  = D3D12_HEAP_TYPE_UPLOAD;
    heapProperty.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperty.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperty.CreationNodeMask      = 1;
    heapProperty.VisibleNodeMask       = 1;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment           = 0;
    resourceDesc.Width               = ALIGN(stride) * num;
    resourceDesc.Height              = 1;
    resourceDesc.DepthOrArraySize    = 1;
    resourceDesc.MipLevels           = 1;
    resourceDesc.Format              = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count    = 1;
    resourceDesc.SampleDesc.Quality  = 0;
    resourceDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

    auto res = dx12::Device::instance().device()->CreateCommittedResource(
        &heapProperty, D3D12_HEAP_FLAG_NONE,
        &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(gpuResource_.GetAddressOf()));

    if (FAILED(res)) {
        ASSERT(false, "GPUリソースの作成に失敗");
        return false;
    }

    alignedStride_ = ALIGN(stride);
    num_           = num;
    size_          = num_ * alignedStride_;

	setName("コンスタントバッファ");

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	オフセットを取得する
 * @param	index			バッファのインデックス
 * @return	インデックスに対応するオフセット
 */
uint64_t ConstantBufferResource::offset(uint32_t index) const noexcept {
    return stride() * index;
}

}  // namespace dx12::resource
