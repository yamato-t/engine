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

    auto res = dx12::Device::instance().device()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                                          IID_PPV_ARGS(gpuResource_.GetAddressOf()));
    if (FAILED(res)) {
        ASSERT(false, "コンスタントバッファの作成に失敗");
        return false;
    }

    gpuResource_->Map(0, nullptr, reinterpret_cast<void**>(data));

    stride_ = stride;
    num_    = num;

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
        cbvDesc.BufferLocation                  = gpuResource_->GetGPUVirtualAddress() + i * ALIGN(stride_);
        cbvDesc.SizeInBytes                     = ALIGN(stride_);

        D3D12_CPU_DESCRIPTOR_HANDLE handle{};
        handle.ptr = handle_.cpuHandle_ + (i * handle_.incrementSize_);
        dx12::Device::instance().device()->CreateConstantBufferView(&cbvDesc, handle);
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList			設定先のコマンドリスト
 * @param	index				コンスタントバッファのインデックス
 * @param	rootParameterIndex	ルートパラメータのインデックス
 */
void ConstantBufferResource::setToCommandList(dx12::CommandList& commandList, uint32_t index, uint32_t rootParameterIndex) noexcept {
    D3D12_GPU_DESCRIPTOR_HANDLE handle{};
    handle.ptr = handle_.gpuHandle_ + (index * handle_.incrementSize_);
    commandList.get()->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
}

//---------------------------------------------------------------------------------
/**
 * @brief	オフセットを取得する
 * @param	index			コンスタントバッファのインデックス
 * @return	インデックスに対応するオフセット
 */
uint64_t ConstantBufferResource::offset(uint32_t index) const noexcept {
    ASSERT(index < num_, "バッファサイズが不正です");
    return ALIGN(stride_) * index;
}

}  // namespace dx12::resource
