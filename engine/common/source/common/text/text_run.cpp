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
#include "common/text/i_text_run_data.h"
#include "common/text/text_font.h"
#include "common/text/text_locale.h"
#include "common/text/text_manager.h"
#include "common/text/text_pre_vertex.h"
#include "common/text/text_run_data_icon.h"
#include "common/text/text_run_data_string.h"

class TextRunImplementation
{
public:
    TextRunImplementation(
        const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
        const int in_em_size
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
        , _em_size(in_em_size)
    {
        // Nop
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds()
    {
        if (true == _calculate_dirty)
        {
            _calculate_dirty = false;

            _pre_vertex_data = std::make_unique<TextPreVertex>(_em_size);
            VectorInt2 cursor;
            for (const auto& item : _text_run_array)
            {
                item->BuildPreVertexData(
                    *_pre_vertex_data,
                    cursor,
                    _width_limit_enabled,
                    _width_limit,
                    _ui_scale
                    );
            }
            _text_bounds = _pre_vertex_data->GetBounds();
        }

        return _text_bounds;
    }

    const bool SetTextRunArray(
        const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array
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

    const bool SetEMSize(
        const int in_em_size
        )
    {
        bool dirty = false;
        if (_em_size != in_em_size)
        {
            dirty = true;
            _em_size = in_em_size;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool Set(
        const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array,
        const VectorInt2& in_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
        const int in_em_size
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
        if (true == SetEMSize(
            in_em_size
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
                _em_size
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
    std::vector<std::shared_ptr<ITextRunData>> _text_run_array;
    VectorInt2 _container_size;
    bool _width_limit_enabled;
    int _width_limit;
    TextEnum::HorizontalLineAlignment _horizontal_line_alignment;
    TextEnum::VerticalBlockAlignment _vertical_block_alignment;
    int _em_size; // Vertical alignment can use a EM size 
    float _ui_scale;

    bool _calculate_dirty;
    std::unique_ptr<TextPreVertex> _pre_vertex_data;
    VectorInt2 _text_bounds;
    bool _geometry_dirty;
    std::shared_ptr<GeometryGeneric> _geometry;

};

std::shared_ptr<ITextRunData> TextRun::MakeTextRunDataString(
    const std::string& in_string_utf8,
    TextLocale* const in_locale_token,
    TextFont* const in_text_font,
    const int in_font_size,
    const float in_new_line_gap_ratio,
    const VectorFloat4& in_colour
    )
{
    return std::make_shared<TextRunDataString>(
        in_string_utf8,
        in_locale_token,
        in_text_font,
        in_font_size,
        in_new_line_gap_ratio,
        in_colour
        );
}

std::shared_ptr<ITextRunData> TextRun::MakeTextRunDataIcon(
    const int in_icon_id,
    IconFont* const in_icon_font,
    const VectorFloat4& in_colour_tint,
    const float in_new_line_gap_ratio
    )
{
    return std::make_shared<TextRunDataIcon>(
        in_icon_id,
        in_icon_font,
        in_colour_tint,
        in_new_line_gap_ratio
        );
}

TextRun::TextRun(
    const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
    const int in_em_size,
    const float in_ui_scale
    )
{
    _implementation = std::make_unique<TextRunImplementation>(
        in_text_run_array,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment,
        in_em_size,
        in_ui_scale
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
    const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array
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

const bool TextRun::SetEMSize(
    const int _em_size
    )
{
    return _implementation->SetEMSize(
        _em_size
        );
}

const bool TextRun::SetUIScale(
    const float in_ui_scale
    )
{
    return _implementation->SetUIScale(
        in_ui_scale
        );
}

const bool TextRun::Set(
    const std::vector<std::shared_ptr<ITextRunData>>& in_text_run_array,
    const VectorInt2& in_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment,
    const int in_em_size,
    const float in_ui_scale
    )
{
    return _implementation->Set(
        in_text_run_array,
        in_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment,
        in_em_size,
        in_ui_scale
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
