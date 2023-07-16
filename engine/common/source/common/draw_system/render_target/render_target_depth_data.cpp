#include "common/common_pch.h"

#include "common/draw_system/render_target/render_target_depth_data.h"

RenderTargetDepthData::RenderTargetDepthData(
    const DXGI_FORMAT in_format,
    const bool in_clear_depth_on_set,
    const float in_clear_depth,
    const bool in_clear_stencil_on_set,
    const UINT8 in_clear_stencil,
    const bool in_shader_resource
    ) 
    : _format(in_format)
    , _clear_depth_on_set(in_clear_depth_on_set)
    , _clear_depth(in_clear_depth)
    , _clear_stencil_on_set(in_clear_stencil_on_set)
    , _clear_stencil(in_clear_stencil)
    , _shader_resource(in_shader_resource)
{
    return;
}

D3D12_CLEAR_VALUE RenderTargetDepthData::MakeClearValue() const
{
    D3D12_CLEAR_VALUE result;
    result.Format = _format;
    result.DepthStencil.Depth = _clear_depth;
    result.DepthStencil.Stencil = _clear_stencil;
    return result;
}

