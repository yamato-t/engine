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
 * @param	data		データの先頭アドレス
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool ConstantBufferResource::create(void* data, uint32_t stride, uint32_t num) noexcept {

	if (!createCommittedResource(ALIGN(stride), num)) {
        return false;
	}

	setName("コンスタントバッファ");
    gpuResource_->Map(0, nullptr, reinterpret_cast<void**>(data));

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープに登録する
 * @param	descriptorHeap			登録先のヒープ
 */
void ConstantBufferResource::registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept {
    // ヒープ登録ハンドルを取得する
    handle_ = descriptorHeap.registerBuffer(num_);

    for (auto i = 0; i < num_; ++i) {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation                  = gpuResource_->GetGPUVirtualAddress() + i * alignedStride_;
        cbvDesc.SizeInBytes                     = alignedStride_;

        D3D12_CPU_DESCRIPTOR_HANDLE handle{};
        handle.ptr = handle_.cpuHandle_ + (i * handle_.incrementSize_);
        dx12::Device::instance().device()->CreateConstantBufferView(&cbvDesc, handle);
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList			設定先のコマンドリスト
 * @param	rootParameterIndex	ルートパラメータのインデックス
 * @param	index				バッファのインデックス
 */
void ConstantBufferResource::setToCommandList(dx12::CommandList& commandList, uint32_t rootParameterIndex, uint32_t index) noexcept {
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
uint64_t ConstantBufferResource::offset(uint32_t index) const noexcept {
    ASSERT(index < num_, "バッファサイズが不正です");
    ASSERT(alignedStride_ != 0, "サイズが設定されていません");

    return alignedStride_ * index;
}

}  // namespace dx12::resource
