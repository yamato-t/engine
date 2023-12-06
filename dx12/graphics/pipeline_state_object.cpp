#include "dx12/graphics/pipeline_state_object.h"
#include "dx12/graphics/container.h"
#include "dx12/device.h"

#include "utility/crc32.h"

namespace dx12::graphics {

//---------------------------------------------------------------------------------
/**
 * @brief	コンストラクタ
 */
PipelineStateObject::PipelineStateObject() {
    create();
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストに設定する
 * @param	commandList		設定先のコマンドリスト
 */
void PipelineStateObject::setToCommandList(CommandList& commandList) noexcept {
    // パイプラインを設定
    commandList.get()->SetPipelineState(pipelineState_.Get());
}

//---------------------------------------------------------------------------------
/**
 * @brief	パイプラインステートを作成する
 * @return	作成に成功した場合は true
 */
bool PipelineStateObject::create() noexcept {
    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0,    DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    // ラスタライザステート
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesc.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesc.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesc.DepthClipEnable       = true;
    rasterizerDesc.MultisampleEnable     = false;
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.ForcedSampleCount     = 0;
    rasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ブレンドステート
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
        FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    D3D12_BLEND_DESC blendDesc       = {};
    blendDesc.AlphaToCoverageEnable  = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
        blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
    }

    // パイプラインステート
    const auto* rootSignature = Container<RootSignature>::instance().get(TO_HASH("default"));
    const auto* shader        = Container<Shader>::instance().get(TO_HASH("color"));

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout                        = {inputElementDescs, _countof(inputElementDescs)};
    psoDesc.pRootSignature                     = rootSignature->get();
    psoDesc.VS                                 = {shader->vertexShader()->GetBufferPointer(), shader->vertexShader()->GetBufferSize()};
    psoDesc.PS                                 = {shader->pixelShader()->GetBufferPointer(), shader->pixelShader()->GetBufferSize()};
    psoDesc.RasterizerState                    = rasterizerDesc;
    psoDesc.BlendState                         = blendDesc;
    psoDesc.DepthStencilState.DepthEnable      = true;
    psoDesc.DepthStencilState.StencilEnable    = false;
    psoDesc.DepthStencilState.DepthWriteMask   = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc.DepthStencilState.DepthFunc        = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    psoDesc.SampleMask                         = UINT_MAX;
    psoDesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets                   = 1;
    psoDesc.RTVFormats[0]                      = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat                          = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.SampleDesc.Count                   = 1;
    auto res                                   = dx12::Device::instance().device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pipelineState_.GetAddressOf()));
    if (FAILED(res)) {
        ASSERT(false, "パイプラインステートの作成に失敗");
    }

    return true;
}
}  // namespace dx12::graphics
