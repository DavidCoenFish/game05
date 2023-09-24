#include "common/common_pch.h"
#include "common/text/text_run_data_string.h"
#include "common/text/text_font.h"
#include "common/math/math.h"

TextRunDataString::TextRunDataString(
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    TextFont* const in_text_font,
    const int in_font_size,
    const float in_new_line_gap_ratio,
    const VectorFloat4& in_colour
    )
    : _string_utf8(in_string_utf8)
    , _locale_token(in_locale_token)
    , _text_font(in_text_font)
    , _font_size(in_font_size)
    , _new_line_gap_ratio(in_new_line_gap_ratio)
    , _colour(in_colour)
{
    // Nop
}

void TextRunDataString::BuildPreVertexData(
    TextPreVertex& in_out_pre_vertex_data,
    VectorInt2& in_out_cursor,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const float in_ui_scale
    ) const
{
    _text_font->BuildPreVertexData(
        in_out_pre_vertex_data,
        in_out_cursor,
        _string_utf8,
        _locale_token,
        Math::ScaleInt(_font_size, in_ui_scale),
        in_width_limit_enabled,
        in_width_limit,
        Math::ScaleInt(_font_size, (1.0f + _new_line_gap_ratio) * in_ui_scale),
        _colour
        );
}


