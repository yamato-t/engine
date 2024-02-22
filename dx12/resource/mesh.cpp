#include "dx12/resource/mesh.h"
#include "dx12/device.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	頂点バッファを作成する
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool VertexBufferResource::create(uint32_t stride, uint32_t num) noexcept {
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

    setName("頂点バッファ");

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	インデックスバッファを作成する
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool IndexBufferResource::create(uint32_t stride, uint32_t num) noexcept {
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

    setName("インデックスバッファ");

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList		設定先のコマンドリスト
 */
void Mesh::setToCommandList(dx12::CommandList& commandList) noexcept {
    // ポリゴントポロジーの指定
    commandList.get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // 頂点バッファをセット
    commandList.get()->IASetVertexBuffers(0, 1, &vertexView_);

    // インデックスバッファをセット
    commandList.get()->IASetIndexBuffer(&indexView_);
}

//---------------------------------------------------------------------------------
/**
 * @brief	頂点バッファのビューを生成する
 */
void Mesh::createVertexView() noexcept {
    vertexView_.BufferLocation = vertexBufferResource_->get()->GetGPUVirtualAddress();
    vertexView_.StrideInBytes  = vertexBufferResource_->stride();
    vertexView_.SizeInBytes    = vertexBufferResource_->size();
}

//---------------------------------------------------------------------------------
/**
 * @brief	インデックスバッファのビューを生成する
 */
void Mesh::createIndexView() noexcept {
    indexView_.BufferLocation = indexBufferResource_->get()->GetGPUVirtualAddress();
    indexView_.Format         = indexBufferResource_->stride() == 4 ? DXGI_FORMAT_R32_UINT
                                                                    : DXGI_FORMAT_R16_UINT;
    indexView_.SizeInBytes    = indexBufferResource_->size();
}

//---------------------------------------------------------------------------------
/**
 * @brief	頂点バッファのデータを設定する
 */
void Mesh::setVertexData(void* data) noexcept {
    byte* ptr = {};
    vertexBufferResource_->get()->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
    memcpy(ptr, data, vertexBufferResource_->size());
    vertexBufferResource_->get()->Unmap(0, nullptr);
}

//---------------------------------------------------------------------------------
/**
 * @brief	インデックスバッファのデータを設定する
 */
void Mesh::setIndexData(void* data) noexcept {
    byte* ptr = {};
    indexBufferResource_->get()->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
    memcpy(ptr, data, indexBufferResource_->size());
    indexBufferResource_->get()->Unmap(0, nullptr);
}

}  // namespace dx12::resource
