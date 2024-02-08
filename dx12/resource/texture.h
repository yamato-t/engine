#pragma once

#include "dx12/command_list.h"
#include "utility/noncopyable.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"
#include "dx12/resource/gpu_resource.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * テクスチャリソース
 */
class TextureResource final : public GpuResource {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    TextureResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~TextureResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャをファイルから読み込む
     * @param	path				ファイルパス
     * @return	成功した場合は true
     */
    bool create(std::string_view path) noexcept;

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
     * @param	rootParameterIndex		ルートパラメータのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept;

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	GPUリソースを作成する
     * @param	stride		バッファのストライド
     * @param	num			バッファの数
     * @return	作成に成功した場合は true
     */
    bool createCommittedResource(uint32_t stride, uint32_t num) noexcept override;

private:
    D3D12_RESOURCE_DESC            resourcesDesc_{};  ///< リソースフォーマット情報
    DescriptorHeap::RegisterHandle handle_{};         ///< ヒープ登録ハンドル
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * テクスチャ
 */
class Texture final : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    Texture() { resource_ = std::make_unique<TextureResource>(); }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ムーブコンストラクタ
     */
    Texture(Texture&& src) {
        resource_ = std::move(src.resource_);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~Texture() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	テクスチャをファイルから読み込む
     * @param	path				ファイルパス
     * @return	成功した場合は true
     */
    [[nodiscard]] bool create(std::string_view path) noexcept {
        return resource_->create(path);
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
     * @param	commandList				設定先のコマンドリスト
     * @param	rootParameterIndex		ルートパラメータのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept {
        resource_->setToCommandList(commandList, rootParameterIndex);
    }

private:
    std::unique_ptr<TextureResource> resource_{};
};

}  // namespace dx12::resource
