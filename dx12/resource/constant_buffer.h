#pragma once

#include "dx12/device.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"
#include "dx12/resource/gpu_resource.h"
#include "dx12/resource/gpu_obj.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * コンスタントバッファリソース
 */
class ConstantBufferResource final : public GpuResource {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    ConstantBufferResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~ConstantBufferResource() = default;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファを生成する
     * @param	stride		コンスタントバッファのストライド
     * @param	num			コンスタントバッファの数
     * @return	作成に成功した場合は true
     */
    bool create(uint32_t stride, uint32_t num) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	オフセットを取得する
     * @param	index			バッファのインデックス
     * @return	インデックスに対応するオフセット
     */
    [[nodiscard]] uint64_t offset(uint32_t index) const noexcept;

private:
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * コンスタントバッファ
 */
template <class T, uint32_t Num>
class ConstantBuffer final : public GpuObj {
private:
    using type = T;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    ConstantBuffer() { resource_.reset(new ConstantBufferResource()); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ムーブコンストラクタ
     */
    ConstantBuffer(ConstantBuffer&& src) noexcept {
        resource_ = std::move(src.resource_);
        data_     = src.data_;
        handle_   = src.handle_;

        src.data_   = {};
        src.handle_ = {};
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~ConstantBuffer() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファを作成する
     */
    void create() noexcept {
        resource_->create(sizeof(type), Num);
        setDataAddress();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファのデータを取得する
     * @param	index			データインデックス
     * @return	データの参照
     */
    type& operator[](uint32_t index) const noexcept {
        auto* address = reinterpret_cast<byte*>(data_) + resource_->offset(index);
        return *(reinterpret_cast<type*>(address));
    }

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	ビューを生成する
     * @param	descriptorHeap	ビュー（ディスクリプタ）登録先のヒープ
     */
    void createView(DescriptorHeap& descriptorHeap) noexcept override final {
        // ヒープ登録ハンドルを取得する
        handle_ = descriptorHeap.allocate(resource_->num());

        for (auto i = 0; i < resource_->num(); ++i) {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = resource_->get()->GetGPUVirtualAddress() + i * resource_->stride();
            cbvDesc.SizeInBytes    = resource_->stride();

            D3D12_CPU_DESCRIPTOR_HANDLE handle{};
            handle.ptr = handle_.cpuHandle_.ptr + (i * handle_.incrementSize_);
            dx12::Device::instance().device()->CreateConstantBufferView(&cbvDesc, handle);
        }
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	args					コマンドリスト設定時の引数
     */
    void setToCommandList(CommandList& commandList, const Args& args) noexcept override final {
        D3D12_GPU_DESCRIPTOR_HANDLE handle{};
        handle.ptr = handle_.gpuHandle_.ptr + (args.handleIndex_ * handle_.incrementSize_);
        commandList.get()->SetGraphicsRootDescriptorTable(args.rootParameterIndex_, handle);
    }

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	CPUで内容を変更する際のアクセス先アドレスを設定する
     */
    void setDataAddress() noexcept {
        auto* data = reinterpret_cast<void*>(&data_);
        resource_->get()->Map(0, nullptr, reinterpret_cast<void**>(data));
    }

private:
    std::unique_ptr<ConstantBufferResource> resource_{};  ///< コンスタントバッファGPUリソース
    type*                                   data_{};      ///< CPUで内容を変更する際のアクセス先アドレス
    DescriptorHeap::Handle                  handle_{};    ///< ヒープ登録ハンドル
};
}  // namespace dx12::resource
