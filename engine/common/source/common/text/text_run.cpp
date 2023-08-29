#include "common/common_pch.h"
#include "common/text/text_run.h"

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

TextRunData::TextRunData(
    const std::string& in_string_utf8,
    TextLocale* const in_locale_token,
    TextFont* const in_text_font,
    const int in_font_size,
    const int in_new_line_height,
    const VectorFloat4& in_colour
    )
    : _string_utf8(in_string_utf8)
    , _locale_token(in_locale_token)
    , _text_font(in_text_font)
    , _font_size(in_font_size)
    , _new_line_height(in_new_line_height)
    , _colour(in_colour)
{
    // Nop
}

class TextRunImplementation
{
public:
    TextRunImplementation(
        const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
        : _calculate_dirty(true)
        , _geometry_dirty(true)
        , _geometry()
        , _text_run_array(in_text_run_array)
        , _container_size(in_containter_size)
        , _width_limit_enabled(in_width_limit_enabled)
        , _width_limit(in_width_limit)
        , _horizontal_line_alignment(in_horizontal_line_alignment)
        , _vertical_block_alignment(in_vertical_block_alignment)
    {
        // Nop
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds()
    {
        if (true == _calculate_dirty)
        {
            _calculate_dirty = false;

            int font_size = 0;
            if (0 < _text_run_array.size())
            {
                font_size = _text_run_array[0]->_font_size;
            }

            _pre_vertex_data = std::make_unique<TextPreVertex>((float)font_size);
            VectorFloat2 cursor;
            for (const auto& item : _text_run_array)
            {
                const TextRunData& run = *item;
                run._text_font->CalculateTextBounds(
                    *_pre_vertex_data,
                    cursor,
                    run._string_utf8,
                    run._locale_token,
                    run._font_size,
                    _width_limit_enabled,
                    _width_limit,
                    run._new_line_height,
                    run._colour
                    );
            }
            _text_bounds = _pre_vertex_data->GetBounds();
        }

        return _text_bounds;
    }

    const bool SetTextRunArray(
        const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array
        )
    {
        bool dirty = false;
        if (_text_run_array != in_text_run_array)
        {
            dirty = true;
            _text_run_array = in_text_run_array;
            _calculate_dirty = true;
            _geometry_dirty = true;
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
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool Set(
        const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array,
        const VectorInt2& in_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
    {
        bool dirty = false;

        if (true == SetTextRunArray(
            in_text_run_array
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

            int font_size = 0;
            if (0 < _text_run_array.size())
            {
                font_size = _text_run_array[0]->_font_size;
            }

            std::vector<uint8_t> vertex_raw_data;
            _pre_vertex_data->BuildVertexData(
                vertex_raw_data,
                font_size,
                _container_size,
                _horizontal_line_alignment,
                _vertical_block_alignment
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
    std::vector<std::shared_ptr<TextRunData>> _text_run_array;
    VectorInt2 _text_bounds;
    VectorInt2 _container_size;
    bool _width_limit_enabled;
    int _width_limit;
    TextEnum::HorizontalLineAlignment _horizontal_line_alignment;
    TextEnum::VerticalBlockAlignment _vertical_block_alignment;
    std::unique_ptr<TextPreVertex> _pre_vertex_data;

};

TextRun::TextRun(
    const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    _implementation = std::make_unique<TextRunImplementation>(
        in_text_run_array,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
        );
}

TextRun::~TextRun()
{
    // Nop
}

// Get the natural size required by the text using current width limit if enabled
VectorInt2 TextRun::GetTextBounds()
{
    return _implementation->GetTextBounds();
}

const bool TextRun::SetTextRunArray(
    const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array
    )
{
    return _implementation->SetTextRunArray(
        in_text_run_array
        );
}

const bool TextRun::SetTextContainerSize(
    const VectorInt2& in_size
    )
{
    return _implementation->SetTextContainerSize(
        in_size
        );
}

const bool TextRun::SetWidthLimit(
    const bool in_width_limit_enabled,
    const int in_width_limit
    )
{
    return _implementation->SetWidthLimit(
        in_width_limit_enabled,
        in_width_limit
        );
}

const bool TextRun::SetHorizontalLineAlignment(
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
    )
{
    return _implementation->SetHorizontalLineAlignment(
        in_horizontal_line_alignment
        );
}

const bool TextRun::SetVerticalBlockAlignment(
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    return _implementation->SetVerticalBlockAlignment(
        in_vertical_block_alignment
        );
}

const bool TextRun::Set(
    const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array,
    const VectorInt2& in_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    return _implementation->Set(
        in_text_run_array,
        in_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
        );
}

GeometryGeneric* const TextRun::GetGeometry(
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
