#pragma once

#include "dx12/device.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"
#include "utility/noncopyable.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * GPU リソース
 */
class GpuResource : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    GpuResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    GpuResource(GpuResource&& src) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    virtual ~GpuResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	GPUリソースを取得する
     */
    ID3D12Resource* get() const noexcept { return gpuResource_.Get(); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ストライドサイズ（アラインメント済み）を取得する
     */
    uint32_t alignedStride() const noexcept { return alignedStride_; }

    //---------------------------------------------------------------------------------
    /**
     * @brief	バッファ数を取得する
     */
    uint32_t bufferNum() const noexcept { return num_; }

    //---------------------------------------------------------------------------------
    /**
     * @brief	全体のサイズを取得する
     */
    uint32_t size() const noexcept { return size_; }

protected:
    //---------------------------------------------------------------------------------
    /**
     * @brief	GPUリソースを作成する
     * @param	stride		バッファのストライド
     * @param	num			バッファの数
     * @return	作成に成功した場合は true
     */
    [[nodiscard]] virtual bool createCommittedResource(uint32_t stride, uint32_t num) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	GPUリソース名を設定する
     * @param	name		リソース名
     */
    void setName(std::string_view name) noexcept;

public:
    Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource_{};    ///< リソース
    uint32_t                               alignedStride_{};  ///< ストライドサイズ（アラインメント済み）
    uint32_t                               num_{};            ///< バッファ数
    uint32_t                               size_{};           ///< バッファの全体サイズ（ストライド x バッファ数）
};

}  // namespace dx12::resource
