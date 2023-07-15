#include "common/common_pch.h"

#include "common/draw_system/render_target/render_target_format_data.h"

RenderTargetFormatData::RenderTargetFormatData(
    const DXGI_FORMAT in_format,
    const bool in_clear_on_set,
    const VectorFloat4&in_clear_color
    ) 
    : format(in_format)
    , clear_on_set(in_clear_on_set)
    , clear_color(in_clear_color)
{
    return;
}

D3D12_CLEAR_VALUE RenderTargetFormatData::MakeClearValue() const{_d3_d12_clea_r_value result(
{
    format,
    {
        clear_color[0], clear_color[1], clear_color[2], clear_color[3] }})
        ; return result;
    }
