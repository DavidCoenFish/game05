#include "common/common_pch.h"
#include "common/text/icon_font.h"

#include "common/draw_system/draw_system.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/dsc_math.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"
#include "common/text/text_block.h"
#include "common/text/text_cell.h"
#include "common/text/text_locale.h"
#include "common/text/text_pre_vertex.h"
#include "common/text/text_texture.h"
#include "common/util/utf8.h"

class IconFontImplementation
{
public:
    IconFontImplementation(
        TextTexture* const in_text_texture
        )
        : _text_texture(in_text_texture)
    {
        // Nop
    }

    ~IconFontImplementation()
    {
        // Nop
    }

    void AddIcon(
        const int in_icon_id,
        const int in_width,
        const int in_height,
        const unsigned char* const in_buffer
        )
    {
        auto cell = _text_texture->MakeIcon(
            in_buffer,
            in_width,
            in_height
            );

        if (nullptr != cell)
        {
            _map_icon_cell[in_icon_id] = cell;
        }
        return;
    }

    void BuildPreVertexData(
        TextPreVertex& in_out_text_pre_vertex,
        VectorInt2& in_out_cursor, // Allow multiple fonts to append pre vertex data
        const int in_icon_id,
        const float in_new_line_gap_ratio,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const float in_ui_scale,
        const VectorFloat4& in_colour_tint
        )
    {
        auto found = _map_icon_cell.find(in_icon_id);
        if (found == _map_icon_cell.end())
        {
            return;
        }
        auto& cell = *(found->second);
        const VectorInt2& cell_width_height = cell.GetWidthHeightRef();
        const int width = static_cast<int>(round(cell_width_height.GetX() * in_ui_scale));
        if ((true == in_width_limit_enabled) &&
            (in_width_limit < (in_out_cursor.GetX() + width)))
        {
            in_out_text_pre_vertex.StartNewLine(in_out_cursor);
        }
        const int colour = DscMath::ConvertColourToInt(in_colour_tint);
        in_out_text_pre_vertex.AddPreVertexScale(
            cell,
            in_out_cursor[0], 
            in_out_cursor[1],
            in_new_line_gap_ratio,
            colour,
            in_ui_scale
            );

        in_out_cursor[0] += cell_width_height[0];

        return;
    }

    void RestIconUsage()
    {
        _map_icon_cell.clear();
        return;
    }

private:
    TextTexture* _text_texture;

    std::map<int, std::shared_ptr<TextCell>> _map_icon_cell;

};

IconFont::IconFont(
    TextTexture* const in_text_texture
    )
{
    _implementation = std::make_unique<IconFontImplementation>(
        in_text_texture
        );
}

IconFont::~IconFont()
{
    _implementation.reset();
}

void IconFont::AddIcon(
    const int in_icon_id,
    const int in_width,
    const int in_height,
    const unsigned char* const in_buffer
    )
{
    _implementation->AddIcon(
        in_icon_id,
        in_width,
        in_height,
        in_buffer
        );
}

void IconFont::BuildPreVertexData(
    TextPreVertex& in_out_text_pre_vertex,
    VectorInt2& in_out_cursor, // Allow multiple fonts to append pre vertex data
    const int in_icon_id,
    const float in_new_line_gap_ratio,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const float in_ui_scale,
    const VectorFloat4& in_colour_tint
    )
{
    _implementation->BuildPreVertexData(
        in_out_text_pre_vertex,
        in_out_cursor,
        in_icon_id,
        in_new_line_gap_ratio,
        in_width_limit_enabled,
        in_width_limit,
        in_ui_scale,
        in_colour_tint
        );
    return;
}

void IconFont::RestIconUsage()
{
    _implementation->RestIconUsage();
    return;
}
