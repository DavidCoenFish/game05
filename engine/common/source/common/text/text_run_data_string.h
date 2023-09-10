#include "common/common_pch.h"
#include "common/math/vector_float4.h"
#include "common/text/i_text_run_data.h"

class TextLocale;
class TextFont;

class TextRunDataString : public ITextRunData
{
public:
    explicit TextRunDataString(
        const std::string& in_string_utf8 = std::string(""),
        TextLocale* const in_locale_token = nullptr,
        TextFont* const in_text_font = nullptr,
        const int in_font_size = 0,
        const int in_new_line_height = 0,
        const VectorFloat4& in_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
        );

private:
    virtual void BuildPreVertexData(
        TextPreVertex& in_out_pre_vertex_data,
        VectorInt2& in_out_cursor,
        const bool in_width_limit_enabled,
        const int in_width_limit
        ) const override;

private:
    std::string _string_utf8;
    TextLocale* const _locale_token;
    TextFont* const _text_font;
    int _font_size;
    VectorFloat4 _colour;
    int _new_line_height;

};