#include "common/common_pch.h"

#include "common/json/json_draw_system_enum.h"
#include "common/json/json_input_element_desc.h"
#include "common/json/json_shader.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    D3D12_RENDER_TARGET_BLEND_DESC,
    BlendEnable,
    LogicOpEnable,
    SrcBlend,
    DestBlend,
    BlendOp,
    SrcBlendAlpha,
    DestBlendAlpha,
    BlendOpAlpha,
    LogicOp,
    RenderTargetWriteMask
    );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    D3D12_BLEND_DESC,
    AlphaToCoverageEnable,
    IndependentBlendEnable,
    RenderTarget
    );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    D3D12_RASTERIZER_DESC,
    FillMode,
    CullMode,
    FrontCounterClockwise,
    DepthBias,
    DepthBiasClamp,
    SlopeScaledDepthBias,
    DepthClipEnable,
    MultisampleEnable,
    AntialiasedLineEnable,
    ForcedSampleCount,
    ConservativeRaster
    );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    D3D12_DEPTH_STENCILOP_DESC,
    StencilFailOp,
    StencilDepthFailOp,
    StencilPassOp,
    StencilFunc
    );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    D3D12_DEPTH_STENCIL_DESC,
    DepthEnable,
    DepthWriteMask,
    DepthFunc,
    StencilEnable,
    StencilReadMask,
    StencilWriteMask,
    FrontFace,
    BackFace
    );
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
// D3D12_STATIC_SAMPLER_DESC,
// Filter,
// AddressU,
// AddressV,
// AddressW,
// MipLODBias,
// MaxAnisotropy,
// ComparisonFunc,
// BorderColor,
// MinLOD,
// MaxLOD,
// ShaderRegister,
// RegisterSpace,
// ShaderVisibility
// );
void to_json(
    in_nlohmann::json&in_j,
    const ShaderPipelineStateData&in_p
    )
{
    in_j;
    in_p;
}

void from_json(
    const in_nlohmann::json&in_j,
    ShaderPipelineStateData&in_p
    )
{
    if (in_j.contains("inputElementDesc"))
    {
        from_json(
            in_j.in_at("inputElementDesc"),
            in_p._input_element_desc_array
            );
    }
    if (in_j.contains("topologyType"))
    {
        in_j.in_at("topologyType") .get_to(in_p._primitive_topology_type);
    }
    if (in_j.contains("depthStencilViewFormat"))
    {
        in_j.in_at("depthStencilViewFormat") .get_to(in_p._depth_stencil_view_format);
    }
    if (in_j.contains("renderTargetFormat"))
    {
        in_j.in_at("renderTargetFormat") .get_to(in_p._render_target_format);
    }
    if (in_j.contains("blendState"))
    {
        in_j.in_at("blendState") .get_to(in_p._blend_state);
    }
    if (in_j.contains("rasterizerState"))
    {
        in_j.in_at("rasterizerState") .get_to(in_p._rasterizer_state);
    }
    if (in_j.contains("depthStencilState"))
    {
        in_j.in_at("depthStencilState") .get_to(in_p._depth_stencil_state);
    }
    return;
}

