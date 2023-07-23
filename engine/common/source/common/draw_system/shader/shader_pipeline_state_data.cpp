#include "common/common_pch.h"

#include "common/draw_system/shader/shader_pipeline_state_data.h"

const D3D12_BLEND_DESC ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied()
{
    D3D12_BLEND_DESC blend_desc;
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        TRUE, //BlendEnable
        FALSE, //LogicOpEnable
        // dest.rgb = src.rgb * src.a + dest.rgb * (1 - src.a)
        D3D12_BLEND_SRC_ALPHA, //SrcBlend 
        D3D12_BLEND_INV_SRC_ALPHA, //DestBlend
        D3D12_BLEND_OP_ADD, //BlendOp
        // dest.a = 1 - (1 - src.a) * (1 - dest.a) [the math works out]
        D3D12_BLEND_INV_DEST_ALPHA, //SrcBlendAlpha
        D3D12_BLEND_ONE, //DestBlendAlpha
        D3D12_BLEND_OP_ADD, //BlendOpAlpha
        D3D12_LOGIC_OP_NOOP, //LogicOp
        D3D12_COLOR_WRITE_ENABLE_ALL, //RenderTargetWriteMask
    };

    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
    {
        blend_desc.RenderTarget[i] = defaultRenderTargetBlendDesc;
    }

    return blend_desc;
}

ShaderPipelineStateData ShaderPipelineStateData::FactoryComputeShader()
{
    return ShaderPipelineStateData(
        std::vector < D3D12_INPUT_ELEMENT_DESC > (),
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
        DXGI_FORMAT_UNKNOWN,
        std::vector < DXGI_FORMAT > (),
        CD3DX12_BLEND_DESC(D3D12_DEFAULT),
        CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
        CD3DX12_DEPTH_STENCIL_DESC(),
        true
        );
}

ShaderPipelineStateData::ShaderPipelineStateData(
    const std::vector < D3D12_INPUT_ELEMENT_DESC >&in_input_element_desc_array,
    const D3D12_PRIMITIVE_TOPOLOGY_TYPE in_primitive_topology_type,
    const DXGI_FORMAT in_depth_stencil_view_format,
    const std::vector < DXGI_FORMAT >&in_render_target_format,
    const D3D12_BLEND_DESC&in_blend_state,
    const D3D12_RASTERIZER_DESC&in_rasterizer_state,
    const D3D12_DEPTH_STENCIL_DESC&in_depth_stencil_state,
    const bool in_compute_shader
    ) 
    : _input_element_desc_array(in_input_element_desc_array)
    , _primitive_topology_type(in_primitive_topology_type)
    , _depth_stencil_view_format(in_depth_stencil_view_format)
    , _render_target_format(in_render_target_format)
    , _blend_state(in_blend_state)
    , _rasterizer_state(in_rasterizer_state)
    , _depth_stencil_state(in_depth_stencil_state)
    , _compute_shader(in_compute_shader)
{
    return;
}

