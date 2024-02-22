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
     * @brief	ムーブコンストラクタ
     */
    GpuResource(GpuResource&& src) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    virtual ~GpuResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	GPUリソース名を設定する
     * @param	name		リソース名
     */
    void setName(std::string_view name) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	リソースフォーマット情報を取得する
     */
    const D3D12_RESOURCE_DESC& desc() const noexcept {
        return resourcesDesc_;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	リソースを取得する
     */
    ID3D12Resource* get() const noexcept {
        return gpuResource_.Get();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	リソースを取得する
     */
    ID3D12Resource* operator->() const noexcept {
        return get();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ストライドを取得する
     */
    uint32_t stride() const noexcept {
        ASSERT(alignedStride_ != 0, "サイズが設定されていません");

        return alignedStride_;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	サイズを取得する
     */
    uint32_t size() const noexcept {
        ASSERT(size_ != 0, "サイズが設定されていません");

		return size_;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	バッファ数を取得する
     */
    uint32_t num() const noexcept {
        ASSERT(num_ != 0, "バッファ数が設定されていません");

		return num_;
    }

protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource_{};    ///< リソース
    D3D12_RESOURCE_DESC                    resourcesDesc_{};  ///< リソースフォーマット情報
    uint32_t                               alignedStride_{};  ///< ストライドサイズ（アラインメント済み）
    uint32_t                               num_{};            ///< バッファ数
    uint32_t                               size_{};           ///< バッファの全体サイズ（ストライド x バッファ数）
};

}  // namespace dx12::resource
