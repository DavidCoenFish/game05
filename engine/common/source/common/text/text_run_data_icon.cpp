#include "common/common_pch.h"
#include "common/text/text_run_data_icon.h"
#include "common/text/icon_font.h"

TextRunDataIcon::TextRunDataIcon(
    const int in_icon_id,
    IconFont* const in_icon_font,
    const VectorFloat4& in_colour_tint,
    const float in_new_line_gap_ratio
    )
    : _icon_id(in_icon_id)
    , _icon_font(in_icon_font)
    , _new_line_gap_ratio(in_new_line_gap_ratio)
    , _colour_tint(in_colour_tint)
{
    // Nop
}

void TextRunDataIcon::BuildPreVertexData(
    TextPreVertex& in_out_pre_vertex_data,
    VectorInt2& in_out_cursor,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const float in_ui_scale
    ) const
{
    _icon_font->BuildPreVertexData(
        in_out_pre_vertex_data,
        in_out_cursor,
        _icon_id,
        _new_line_gap_ratio,
        in_width_limit_enabled,
        in_width_limit,
        in_ui_scale,
        _colour_tint
        );
}
