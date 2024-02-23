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
     * @brief    テクスチャを引数から作成する
     * @param    w h         横と縦のサイズ
     * @param    mipLevel    ミップマップレベル
     * @param    arraySize   テクスチャ配列サイズ
     * @param    format      テクスチャフォーマット
     * @param    color       初期カラー
     * @return    成功した場合は true
     */
    bool create(uint32_t w, uint32_t h, uint32_t mipLevel, uint32_t arraySize, DXGI_FORMAT format, const float color[4] = {}) noexcept;
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
        handle_   = src.handle_;
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
     * @brief    テクスチャを引数から作成する
     * @param    w h         横と縦のサイズ
     * @param    mipLevel    ミップマップレベル
     * @param    arraySize   テクスチャ配列サイズ
     * @param    format      テクスチャフォーマット
     * @return    成功した場合は true
     */
    [[nodiscard]] bool create(uint32_t w, uint32_t h, uint32_t mipLevel, uint32_t arraySize, DXGI_FORMAT format) noexcept {
        return resource_->create(w, h, mipLevel, arraySize, format);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ビューを生成する
     * @param	descriptorHeap	ビュー（ディスクリプタ）登録先のヒープ
     */
    void createView(DescriptorHeap& descriptorHeap) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	rootParameterIndex		ルートパラメータのインデックス
     */
    void setToCommandList(CommandList& commandList, uint32_t rootParameterIndex) noexcept;

private:
    std::unique_ptr<TextureResource> resource_{};  ///< リソース
    DescriptorHeap::Handle           handle_{};    ///< ヒープ登録ハンドル
};

}  // namespace dx12::resource
