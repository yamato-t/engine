#pragma once

#include "dx12/command_list.h"
#include "dx12/resource/gpu_resource.h"
#include "utility/noncopyable.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * 頂点バッファリソース
 */
class VertexBufferResource final : public GpuResource {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    VertexBufferResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~VertexBufferResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	頂点バッファを作成する
     * @param	stride		バッファのストライド
     * @param	num			バッファの数
     * @return	作成に成功した場合は true
     */
    bool create(uint32_t stride, uint32_t num) noexcept;
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * インデックスバッファリソース
 */
class IndexBufferResource final : public GpuResource {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    IndexBufferResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~IndexBufferResource() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスバッファを作成する
     * @param	stride		バッファのストライド
     * @param	num			バッファの数
     * @return	作成に成功した場合は true
     */
    bool create(uint32_t stride, uint32_t num) noexcept;
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * メッシュ
 */
class Mesh final : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    Mesh() {
        vertexBufferResource_.reset(new VertexBufferResource());
        indexBufferResource_.reset(new IndexBufferResource());
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~Mesh() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	頂点バッファを作成する
     * @param	data		データの先頭アドレス
     */
    template <class vertexFormat, uint32_t Num>
    void createVertexBuffer(vertexFormat (&data)[Num]) noexcept {
        vertexBufferResource_->create(sizeof(vertexFormat), Num);

        setVertexData(reinterpret_cast<void*>(data));
        createVertexView();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	頂点バッファを作成する
     * @param	data		データの先頭アドレス
     * @param	num			データ数
     */
    template <class vertexFormat>
    void createVertexBuffer(vertexFormat* data, uint32_t num) noexcept {
        vertexBufferResource_->create(sizeof(vertexFormat), num);

        setVertexData(reinterpret_cast<void*>(data));
        createVertexView();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスバッファを作成する
     * @param	data		データの先頭アドレス
     */
    template <class indexFormat, uint32_t Num>
    void createIndexBuffer(indexFormat (&data)[Num]) noexcept {
        indexBufferResource_->create(sizeof(indexFormat), Num);

        setIndexData(reinterpret_cast<void*>(data));
        createIndexView();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスバッファを作成する
     * @param	data		データの先頭アドレス
     * @param	num			データ数
     */
    template <class indexFormat>
    void createIndexBuffer(indexFormat* data, uint32_t num) noexcept {
        indexBufferResource_->create(sizeof(indexFormat), num);

        setIndexData(reinterpret_cast<void*>(data));
        createIndexView();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスバッファの要素数を取得する
     * @return 	インデックスバッファの要素数
     */
    uint32_t getIndexBufferNum() const noexcept {
        return indexBufferResource_->num();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList		設定先のコマンドリスト
     */
    void setToCommandList(CommandList& commandList) noexcept;

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	頂点バッファのビューを生成する
     */
    void createVertexView() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスバッファのビューを生成する
     */
    void createIndexView() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	頂点バッファのデータを設定する
     */
    void setVertexData(void* data) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスバッファのデータを設定する
     */
    void setIndexData(void* data) noexcept;

private:
    std::unique_ptr<VertexBufferResource> vertexBufferResource_{};  ///< 頂点バッファリソース
    std::unique_ptr<IndexBufferResource>  indexBufferResource_{};   ///< インデックスバッファリソース

    D3D12_VERTEX_BUFFER_VIEW vertexView_{};  ///< 頂点バッファビュー
    D3D12_INDEX_BUFFER_VIEW  indexView_{};   ///< インデックスバッファビュー
};
}  // namespace dx12::resource
