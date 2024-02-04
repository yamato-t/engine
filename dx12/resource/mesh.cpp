#include "dx12/resource/mesh.h"
#include "dx12/device.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief	頂点バッファを作成する
 * @param	data		データの先頭アドレス
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool VertexBufferResource::create(void* data, uint32_t stride, uint32_t num) noexcept {
    if (!createCommittedResource(stride, num)) {
        return false;
    }
    setName("頂点バッファ");

    byte* ptr = {};
    gpuResource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
    memcpy(ptr, data, size_);
    gpuResource_->Unmap(0, nullptr);

    // 頂点バッファビュー作成
    view_.BufferLocation = gpuResource_->GetGPUVirtualAddress();
    view_.StrideInBytes  = alignedStride_;
    view_.SizeInBytes    = size_;

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList		設定先のコマンドリスト
 */
void VertexBufferResource::setToCommandList(dx12::CommandList& commandList) noexcept {
    // ポリゴントポロジーの指定
    commandList.get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // 頂点バッファをセット
    commandList.get()->IASetVertexBuffers(0, 1, &view_);
}

//---------------------------------------------------------------------------------
/**
 * @brief	インデックスバッファを作成する
 * @param	data		データの先頭アドレス
 * @param	stride		バッファのストライド
 * @param	num			バッファの数
 * @return	作成に成功した場合は true
 */
bool IndexBufferResource::create(void* data, uint32_t stride, uint32_t num) noexcept {

    if (!createCommittedResource(stride, num)) {
        return false;
    }
    setName("インデックスバッファ");


    byte* ptr = {};
    gpuResource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
    memcpy(ptr, data, size_);
    gpuResource_->Unmap(0, nullptr);

    // インデックスバッファビュー作成
    view_.BufferLocation = gpuResource_->GetGPUVirtualAddress();
    view_.Format         = alignedStride_ == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
    view_.SizeInBytes    = size_;

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList		設定先のコマンドリスト
 */
void IndexBufferResource::setToCommandList(dx12::CommandList& commandList) noexcept {
    // インデックスバッファをセット
    commandList.get()->IASetIndexBuffer(&view_);
}

//---------------------------------------------------------------------------------
/**
 * @brief	インデックスバッファの要素数を取得する
 * @return	要素数
 */
uint32_t IndexBufferResource::getNum() const noexcept {
    return num_;
}

}  // namespace dx12::resource
