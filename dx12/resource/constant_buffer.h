#pragma once

#include "dx12/device.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"
#include "dx12/resource/gpu_resource.h"

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
     * @param	data		データの先頭アドレス
     * @param	stride		コンスタントバッファのストライド
     * @param	num			コンスタントバッファの数
     * @return	作成に成功した場合は true
     */
    bool create(void* data, uint32_t stride, uint32_t num) noexcept;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープに登録する
     * @param	descriptorHeap	登録先のヒープ
     */
    void registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	rootParameterIndex		ルートパラメータのインデックス
     * @param	index					バッファのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex, uint32_t index = 0) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	オフセットを取得する
     * @param	index			バッファのインデックス
     * @return	インデックスに対応するオフセット
     */
    [[nodiscard]] uint64_t offset(uint32_t index) const noexcept;

private:
    DescriptorHeap::RegisterHandle handle_{};  ///< ヒープ登録ハンドル
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * コンスタントバッファ
 */
template <class T, uint32_t Num>
class ConstantBuffer final : public utility::Noncopyable {
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

        src.data_ = {};
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
        resource_->create(reinterpret_cast<void*>(&data_), sizeof(type), Num);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープに登録する
     * @param	descriptorHeap			登録先のヒープ
     */
    void registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept {
        resource_->registerToDescriptorHeap(descriptorHeap);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList			設定先のコマンドリスト
     * @param	rootParameterIndex	ルートパラメータのインデックス
     * @param	index				コンスタントバッファのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex, uint32_t index) noexcept {
        resource_->setToCommandList(commandList, rootParameterIndex, index);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファのデータを取得する
     * @param	index			データインデックス
     * @return	データの参照
     */
    type& operator[](uint32_t index) const noexcept {
        auto* address = reinterpret_cast<char*>(data_) + resource_->offset(index);
        return *(reinterpret_cast<type*>(address));
    }

private:
    std::unique_ptr<ConstantBufferResource> resource_{};  ///< コンスタントバッファGPUリソース
    type*                                   data_{};      ///< CPUで内容を変更する際のアクセス先アドレス
};
}  // namespace dx12::resource
