#include "dx12/resource/gpu_resource.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList			設定先のコマンドリスト
 * @param	rootParameterIndex	ルートパラメータのインデックス
 * @param	index				バッファのインデックス
 */
void GpuResource::setToCommandList(dx12::CommandList& commandList, uint32_t rootParameterIndex, uint32_t index) noexcept {
    D3D12_GPU_DESCRIPTOR_HANDLE handle{};
    handle.ptr = handle_.gpuHandle_ + (index * handle_.incrementSize_);
    commandList.get()->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
}

//---------------------------------------------------------------------------------
/**
 * @brief	オフセットを取得する
 * @param	index			バッファのインデックス
 * @return	インデックスに対応するオフセット
 */
uint64_t GpuResource::offset(uint32_t index) const noexcept {
    ASSERT(index < num_, "バッファサイズが不正です");
    ASSERT(alignedSize_ != 0, "サイズが設定されていません");

    return alignedSize_ * index;
}

}  // namespace dx12::resource
