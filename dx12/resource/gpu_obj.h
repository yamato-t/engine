#pragma once

#include "dx12/device.h"
#include "dx12/command_list.h"
#include "dx12/descriptor_heap.h"
#include "utility/noncopyable.h"

namespace dx12::resource {

//---------------------------------------------------------------------------------
/**
 * @brief
 * GPU オブジェクト
 */
class GpuObj : public utility::Noncopyable {
public:

	//---------------------------------------------------------------------------------
    /**
     * @brief  コマンド設定の引数情報
     */
    struct Args {
        uint32_t rootParameterIndex_{};
        uint32_t handleIndex_{};
    };

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    GpuObj() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    virtual ~GpuObj() = default;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	ビューを生成する
     * @param	descriptorHeap	ビュー（ディスクリプタ）登録先のヒープ
     */
    virtual void createView(DescriptorHeap& descriptorHeap) noexcept = 0;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList				設定先のコマンドリスト
     * @param	args					コマンドリスト設定時の引数
     */
    virtual void setToCommandList(CommandList& commandList, const Args& args) noexcept = 0;
};
}  // namespace dx12::resource
