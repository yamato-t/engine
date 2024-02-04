#include "dx12/resource/gpu_resource.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	コンストラクタ
 */
GpuResource::GpuResource(GpuResource&& src) noexcept {

    gpuResource_ = std::move(src.gpuResource_);

    alignedStride_ = src.alignedStride_;
    num_           = src.num_;
    size_          = src.size_;

    src.gpuResource_.Reset();
    src.alignedStride_ = {};
    src.num_           = {};
    src.size_          = {};
}

//---------------------------------------------------------------------------------
/**
 * @brief	GPUリソースを作成する
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool GpuResource::createCommittedResource(uint32_t stride, uint32_t num) noexcept {
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
    resourceDesc.Width               = stride * num;
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

    alignedStride_ = stride;
    num_           = num;
    size_          = num_ * alignedStride_;

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	GPUリソース名を設定する
 * @param	name		リソース名
 */
void GpuResource::setName(std::string_view name) noexcept {
    ASSERT(size_ != 0, "GPUリソースが作成されていない可能性があります");

    auto temp = std::wstring(name.begin(), name.end());
    gpuResource_->SetName(temp.data());
}

}  // namespace dx12::resource
