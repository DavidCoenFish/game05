#include "common/common_pch.h"
#include "common/math/vector_float4.h"
#include "common/text/i_text_run_data.h"

class IconFont;

class TextRunDataIcon : public ITextRunData
{
public:
    explicit TextRunDataIcon(
        const int in_icon_id = 0,
        IconFont* const in_icon_font = nullptr,
        const int in_new_line_height = 0,
        const VectorFloat4& in_colour_tint = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
        );

private:
    virtual void BuildPreVertexData(
        TextPreVertex& in_out_pre_vertex_data,
        VectorInt2& in_out_cursor,
        const bool in_width_limit_enabled,
        const int in_width_limit
        ) const override;

private:
    int _icon_id;
    IconFont* const _icon_font;
    int _new_line_height;
    VectorFloat4 _colour_tint;

};