#include "common/common_pch.h"
#include "common/text/text_block.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/text/text_font.h"
#include "common/text/text_locale.h"
#include "common/text/text_manager.h"
#include "common/text/text_pre_vertex.h"

class TextBlockImplementation
{
public:
    TextBlockImplementation(
        TextFont& in_text_font,
        const int in_font_size,
        const float in_new_line_gap_ratio,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
        const VectorFloat4& in_colour,
        const float in_ui_scale
        )
        : _calculate_dirty(true)
        , _geometry_dirty(true)
        , _geometry()
        , _text_font(&in_text_font)
        , _font_size(in_font_size)
        , _new_line_gap_ratio(in_new_line_gap_ratio)
        , _string_utf8(in_string_utf8)
        , _locale_token(in_locale_token)
        , _container_size(in_containter_size)
        , _width_limit_enabled(in_width_limit_enabled)
        , _width_limit(in_width_limit)
        , _horizontal_line_alignment(in_horizontal_line_alignment)
        , _vertical_block_alignment(in_vertical_block_alignment)
        , _colour(in_colour)
        , _ui_scale(in_ui_scale)
    {
        // Nop
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds()
    {
        if (true == _calculate_dirty)
        {
            _calculate_dirty = false;

            _pre_vertex_data = std::make_unique<TextPreVertex>(_font_size);
            VectorInt2 cursor;
            _text_font->BuildPreVertexData(
                *_pre_vertex_data,
                cursor,
                _string_utf8,
                _locale_token,
                static_cast<int>(round(_font_size * _ui_scale)),
                _width_limit_enabled,
                _width_limit,
                static_cast<int>(round((_font_size + (_font_size * _new_line_gap_ratio)) * _ui_scale))
                );
            _text_bounds = _pre_vertex_data->GetBounds(_vertical_block_alignment);
        }

        return _text_bounds;
    }

    TextFont* const GetFont() const
    {
        return _text_font;
    }

    const bool SetFont(
        TextFont& in_text_font
        )
    {
        bool dirty = false;
        if (_text_font != &in_text_font)
        {
            dirty = true;
            _calculate_dirty = true;
            _geometry_dirty = true;
            _text_font = &in_text_font;
        }
        return dirty;
    }

    const bool SetFontSize(
        const int in_font_size
        )
    {
        bool dirty = false;
        if (_font_size != in_font_size)
        {
            dirty = true;
            _calculate_dirty = true;
            _geometry_dirty = true;
            _font_size = in_font_size;
        }
        return dirty;
    }

    const bool SetNewLineGapRatio(
        const float in_new_line_gap_ratio
        )
    {
        bool dirty = false;
        if (_new_line_gap_ratio != in_new_line_gap_ratio)
        {
            dirty = true;
            _calculate_dirty = true;
            _geometry_dirty = true;
            _new_line_gap_ratio = in_new_line_gap_ratio;
        }
        return dirty;
    }

    const bool SetText(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token
        )
    {
        bool dirty = false;
        if ((_string_utf8 != in_string_utf8) ||
            (_locale_token != in_locale_token))
        {
            dirty = true;
            _calculate_dirty = true;
            _geometry_dirty = true;
            _string_utf8 = in_string_utf8;
            _locale_token = in_locale_token;
        }
        return dirty;
    }

    const bool SetTextContainerSize(
        const VectorInt2& in_container_size
        )
    {
        bool dirty = false;
        if (_container_size != in_container_size)
        {
            dirty = true;
            _container_size = in_container_size;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool GetWidthLimitEnabled() const
    {
        return _width_limit_enabled;
    }

    const bool SetWidthLimitEnabled(
        const bool in_width_limit_enabled
        )
    {
        bool dirty = false;
        if (_width_limit_enabled != in_width_limit_enabled)
        {
            dirty = true;
            _width_limit_enabled = in_width_limit_enabled;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return dirty;
    }


    const bool SetWidthLimit(
        const bool in_width_limit_enabled,
        const int in_width_limit
        )
    {
        bool dirty = false;
        if ((_width_limit_enabled != in_width_limit_enabled) ||
            (_width_limit != in_width_limit))
        {
            dirty = true;
            _width_limit_enabled = in_width_limit_enabled;
            _width_limit = in_width_limit;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
        )
    {
        bool dirty = false;
        if (_horizontal_line_alignment != in_horizontal_line_alignment)
        {
            dirty = true;
            _horizontal_line_alignment = in_horizontal_line_alignment;
            //_calculate_dirty = true;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
    {
        bool dirty = false;
        if (_vertical_block_alignment != in_vertical_block_alignment)
        {
            dirty = true;
            _vertical_block_alignment = in_vertical_block_alignment;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool SetColour(
        const VectorFloat4& in_colour
        )
    {
        bool dirty = false;
        if (_colour != in_colour)
        {
            dirty = true;
            _colour = in_colour;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool SetUIScale(
        const float in_ui_scale
        )
    {
        bool dirty = false;
        if (_ui_scale != in_ui_scale)
        {
            dirty = true;
            _ui_scale = in_ui_scale;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool Set(
        TextFont& in_text_font,
        const int in_font_size,
        const float in_new_line_gap_ratio, // new line = in_font_size + (in_font_size * in_new_line_gap_ratio)
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const VectorInt2& in_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
        const VectorFloat4& in_colour,
        const float in_ui_scale
        )
    {
        bool dirty = false;
        if (true == SetFont(
            in_text_font
            ))
        {
            dirty = true;
        }
        if (true == SetFontSize(
            in_font_size
            ))
        {
            dirty = true;
        }
        if (true == SetNewLineGapRatio(
            in_new_line_gap_ratio
            ))
        {
            dirty = true;
        }
        if (true == SetText(
            in_string_utf8,
            in_locale_token
            ))
        {
            dirty = true;
        }
        if (true == SetTextContainerSize(
            in_size
            ))
        {
            dirty = true;
        }
        if (true == SetWidthLimit(
            in_width_limit_enabled,
            in_width_limit
            ))
        {
            dirty = true;
        }
        if (true == SetHorizontalLineAlignment(
            in_horizontal_line_alignment
            ))
        {
            dirty = true;
        }
        if (true == SetVerticalBlockAlignment(
            in_vertical_block_alignment
            ))
        {
            dirty = true;
        }
        if (true == SetColour(
            in_colour
            ))
        {
            dirty = true;
        }
        if (true == SetUIScale(
            in_ui_scale
            ))
        {
            dirty = true;
        }
        return dirty;
    }

    GeometryGeneric* const GetGeometry(
        bool& out_geometry_dirty,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        )
    {
        GetTextBounds();

        if (true == _geometry_dirty)
        {
            _geometry_dirty = false;
            out_geometry_dirty = true;

            std::vector<uint8_t> vertex_raw_data;
            _pre_vertex_data->BuildVertexData(
                vertex_raw_data,
                _container_size,
                _horizontal_line_alignment,
                _vertical_block_alignment,
                _font_size
                );

            if (nullptr == _geometry)
            {
                _geometry = in_draw_system->MakeGeometryGeneric(
                    in_command_list,
                    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                    TextManager::GetInputElementDesc(),
                    vertex_raw_data,
                    12
                    );
            }
            else
            {
                in_draw_system->UpdateGeometryGeneric(
                    in_command_list,
                    _geometry.get(),
                    vertex_raw_data
                    );
            }
        }

        return _geometry.get();
    }

private:
    bool _calculate_dirty;
    bool _geometry_dirty;
    std::shared_ptr<GeometryGeneric> _geometry;
    TextFont* _text_font;
    int _font_size;
    float _new_line_gap_ratio;
    std::string _string_utf8;
    const TextLocale* _locale_token;
    VectorInt2 _text_bounds;
    VectorInt2 _container_size;
    bool _width_limit_enabled;
    int _width_limit;
    TextEnum::HorizontalLineAlignment _horizontal_line_alignment;
    TextEnum::VerticalBlockAlignment _vertical_block_alignment;
    VectorFloat4 _colour;
    float _ui_scale;

    std::unique_ptr<TextPreVertex> _pre_vertex_data;

};

TextBlock::TextBlock(
    TextFont& in_text_font,
    const int in_font_size,
    const float in_new_line_gap_ratio, // new line = in_font_size + (in_font_size * in_new_line_gap_ratio)
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
    const VectorFloat4& in_colour,
    const float in_ui_scale
    )
{
    _implementation = std::make_unique<TextBlockImplementation>(
        in_text_font,
        in_font_size,
        in_new_line_gap_ratio,
        in_string_utf8,
        in_locale_token,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment,
        in_colour,
        in_ui_scale
        );
}

TextBlock::~TextBlock()
{
    // Nop
}

// Get the natural size required by the text using current width limit if enabled
VectorInt2 TextBlock::GetTextBounds()
{
    return _implementation->GetTextBounds();
}

TextFont* const TextBlock::GetFont() const
{
    return _implementation->GetFont();
}

const bool TextBlock::SetFont(
    TextFont& in_text_font
    )
{
    return _implementation->SetFont(
        in_text_font
        );
}

const bool TextBlock::SetFontSize(
    const int in_font_size
    )
{
    return _implementation->SetFontSize(
        in_font_size
        );
}

const bool TextBlock::SetNewLineGapRatio(
    const float in_new_line_gap_ratio // new line = in_font_size + (in_font_size * in_new_line_gap_ratio)
    )
{
    return _implementation->SetNewLineGapRatio(
        in_new_line_gap_ratio
        );
}

const bool TextBlock::SetText(
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token
    )
{
    return _implementation->SetText(
        in_string_utf8,
        in_locale_token
        );
}

const bool TextBlock::SetTextContainerSize(
    const VectorInt2& in_size
    )
{
    return _implementation->SetTextContainerSize(
        in_size
        );
}

const bool TextBlock::GetWidthLimitEnabled() const
{
    return _implementation->GetWidthLimitEnabled();
}

const bool TextBlock::SetWidthLimitEnabled(
    const bool in_width_limit_enabled
    )
{
    return _implementation->SetWidthLimitEnabled(
        in_width_limit_enabled
        );
}

const bool TextBlock::SetWidthLimit(
    const bool in_width_limit_enabled,
    const int in_width_limit
    )
{
    return _implementation->SetWidthLimit(
        in_width_limit_enabled,
        in_width_limit
        );
}

const bool TextBlock::SetHorizontalLineAlignment(
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
    )
{
    return _implementation->SetHorizontalLineAlignment(
        in_horizontal_line_alignment
        );
}

const bool TextBlock::SetVerticalBlockAlignment(
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    return _implementation->SetVerticalBlockAlignment(
        in_vertical_block_alignment
        );
}

const bool TextBlock::SetColour(
    const VectorFloat4& in_colour
    )
{
    return _implementation->SetColour(
        in_colour
        );
}

const bool TextBlock::SetUIScale(
    const float in_ui_scale
    )
{
    return _implementation->SetUIScale(
        in_ui_scale
        );
}

const bool TextBlock::Set(
    TextFont& in_text_font,
    const int in_font_size,
    const float in_new_line_gap_ratio, // new line = in_font_size + (in_font_size * in_new_line_gap_ratio)
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const VectorInt2& in_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
    const VectorFloat4& in_colour,
    const float in_ui_scale
    )
{
    return _implementation->Set(
        in_text_font,
        in_font_size,
        in_new_line_gap_ratio, 
        in_string_utf8,
        in_locale_token,
        in_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment,
        in_colour,
        in_ui_scale
        );
}

GeometryGeneric* const TextBlock::GetGeometry(
    bool& out_geometry_dirty,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    return _implementation->GetGeometry(
        out_geometry_dirty,
        in_draw_system,
        in_command_list
        );
}
