#pragma once

#include "dx12/device.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"

#include "utility/noncopyable.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * コンスタントバッファリソース
 */
class ConstantBufferResource final : public utility::Noncopyable {
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

    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファを生成する
     * @param	data		データの先頭アドレス
     * @param	stride		コンスタントバッファのストライド
     * @param	num			コンスタントバッファの数
     * @return	作成に成功した場合は true
     */
    bool create(void* data, uint32_t stride, uint32_t num) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープに登録する
     * @param	descriptorHeap			登録先のヒープ
     */
    void registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	index					コンスタントバッファのインデックス
     * @param	descriptorTableIndex	ディスクリプタテーブルのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t index, uint32_t descriptorTableIndex) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	オフセットを取得する
     * @param	index			コンスタントバッファのインデックス
     * @return	インデックスに対応するオフセット
     */
    uint64_t offset(uint32_t index) const noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource_{};  ///< リソース
    uint32_t                               stride_{};       ///< バッファのストライド
    uint32_t                               num_{};          ///< バッファ数
    DescriptorHeap::RegisterHandle         handle_{};       ///< ヒープ登録ハンドル
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * コンスタントバッファ
 */
template <class T, uint32_t Num>
class ConstantBuffer final : utility::Noncopyable {
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
    void createBuffer() noexcept { resource_->create(reinterpret_cast<void*>(&data_), sizeof(type), Num); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタヒープに登録する
     * @param	descriptorHeap			登録先のヒープ
     */
    void registerToDescriptorHeap(DescriptorHeap& descriptorHeap) noexcept { resource_->registerToDescriptorHeap(descriptorHeap); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	index					コンスタントバッファのインデックス
     * @param	descriptorTableIndex	ディスクリプタテーブルのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t index, uint32_t descriptorTableIndex) noexcept { resource_->setToCommandList(commandList, index, descriptorTableIndex); }

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
    std::unique_ptr<ConstantBufferResource> resource_ = {};
    type*                                   data_     = {};  ///< CPUで内容を変更する際のアクセス先アドレス
};
}  // namespace dx12::resource
