#include "common/common_pch.h"
#include "common/text/text_run_data_icon.h"
#include "common/text/icon_font.h"

TextRunDataIcon::TextRunDataIcon(
    const int in_icon_id, // Zero if you are using a string and not an icon
    IconFont* const in_icon_font,
    const int in_new_line_height,
    const VectorFloat4& in_colour_tint
    )
    : _icon_id(in_icon_id)
    , _icon_font(in_icon_font)
    , _new_line_height(in_new_line_height)
    , _colour_tint(in_colour_tint)
{
    // Nop
}

void TextRunDataIcon::BuildPreVertexData(
    TextPreVertex& in_out_pre_vertex_data,
    VectorInt2& in_out_cursor,
    const bool in_width_limit_enabled,
    const int in_width_limit
    ) const
{
    _icon_font->BuildPreVertexData(
        in_out_pre_vertex_data,
        in_out_cursor,
        _icon_id,
        _new_line_height,
        in_width_limit_enabled,
        in_width_limit,
        _colour_tint
        );
}
