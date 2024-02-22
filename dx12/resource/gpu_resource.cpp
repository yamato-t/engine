#include "dx12/resource/gpu_resource.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	ムーブコンストラクタ
 */
GpuResource::GpuResource(GpuResource&& src) noexcept {
    gpuResource_   = std::move(src.gpuResource_);
    resourcesDesc_ = src.resourcesDesc_;
    alignedStride_ = src.alignedStride_;
    num_           = src.num_;
    size_          = src.size_;

    src.gpuResource_.Reset();
    src.resourcesDesc_ = {};
    src.alignedStride_ = {};
    src.num_           = {};
    src.size_          = {};
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
