#pragma once

#include "dx12/command_list.h"
#include "utility/noncopyable.h"

namespace dx12::graphics {
//---------------------------------------------------------------------------------
/**
 * @brief
 * ルートシグネチャ
 */
class RootSignature : public utility::Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    RootSignature();

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~RootSignature() = default;

	//---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストに設定する
     * @param	commandList		設定先のコマンドリスト
     */
    void setToCommandList(CommandList& commandList) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ルートシグネチャを取得する
     * @return	ID3D12RootSignature
     */
    ID3D12RootSignature* get() const noexcept;

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	ルートシグネチャを作成する
     * @return	作成に成功した場合は true
     */
    bool create() noexcept;


private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_{};  ///< ルートシグネチャ
};
}  // namespace dx12::graphics
