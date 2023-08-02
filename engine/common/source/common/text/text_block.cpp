#include "common/common_pch.h"
#include "common/text/text_block.h"
#include "common/text/text_locale.h"
#include "common/text/text_manager.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float4.h"

class TextBlockImplementation
{
public:
    TextBlockImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment,
        const std::vector<uint8_t>& in_vertex_raw_data
        )
        : _geometry_dirty(false)
        , _geometry()
        , _string_utf8(in_string_utf8)
        , _locale_token(in_locale_token)
        , _container_size(in_containter_size)
        , _width_limit_enabled(in_width_limit_enabled)
        , _width_limit(in_width_limit)
        , _horizontal_line_alignment(in_horizontal_line_alignment)
        , _vertical_block_alignment(in_vertical_block_alignment)
    {
        _geometry = in_draw_system->MakeGeometryGeneric(
            in_command_list,
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
            TextManager::GetInputElementDesc(),
            in_vertex_raw_data,
            8
        );

    }

    void SetText(
        const std::string& in_string_utf8
        )
    {
        if (_string_utf8 != in_string_utf8)
        {
            _string_utf8 = in_string_utf8;
            _geometry_dirty = true;
        }
        return;
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds() const
    {
        return _text_bounds;
    }

    void SetTextContainerSize(
        const VectorInt2& in_container_size
        )
    {
        if (_container_size != in_container_size)
        {
            _container_size = in_container_size;
            _geometry_dirty = true;
        }
        return;
    }

    void SetWidthLimit(
        const bool in_width_limit_enabled,
        const int in_width_limit
        )
    {
        if ((_width_limit_enabled != in_width_limit_enabled) ||
            (_width_limit != in_width_limit))
        {
            _width_limit_enabled = in_width_limit_enabled;
            _width_limit = in_width_limit;
            _geometry_dirty = true;
        }
        return;
    }

    void SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment
        )
    {
        if (_horizontal_line_alignment != in_horizontal_line_alignment)
        {
            _horizontal_line_alignment = in_horizontal_line_alignment;
            _geometry_dirty = true;
        }
        return;
    }

    void SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        )
    {
        if (_vertical_block_alignment != in_vertical_block_alignment)
        {
            _vertical_block_alignment = in_vertical_block_alignment;
            _geometry_dirty = true;
        }
        return;
    }

    // Which is better, two pass, or one function that coould do two things
    void Update(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
    )
    {
        //TODO, move up?
        std::vector<uint8_t> vertex_raw_data;

        in_draw_system->UpdateGeometryGeneric(
            in_command_list,
            _geometry.get(),
            vertex_raw_data
            );
        _geometry_dirty = false;
        return;
    }

    GeometryGeneric* const GetGeometry() const
    {
        return _geometry.get();
    }

    const bool GetGeometryDirty() const 
    {
        return _geometry_dirty;
    }

private:
    bool _geometry_dirty;
    std::shared_ptr<GeometryGeneric> _geometry;
    std::string _string_utf8;
    const TextLocale* const _locale_token;
    VectorInt2 _text_bounds;
    VectorInt2 _container_size;
    bool _width_limit_enabled;
    int _width_limit;
    TextEnum::HorizontalLineAlignment::Enum _horizontal_line_alignment;
    TextEnum::VerticalBlockAlignment::Enum _vertical_block_alignment;

};

TextBlock::TextBlock(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment,
    const std::vector<uint8_t>& in_vertex_raw_data
    )
{
    _implementation = std::make_unique<TextBlockImplementation>(
        in_draw_system,
        in_command_list,
        in_string_utf8,
        in_locale_token,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment,
        in_vertex_raw_data
        );
}

TextBlock::~TextBlock()
{
    // Nop
}

void TextBlock::SetText(
    const std::string& in_string_utf8
    )
{
    _implementation->SetText(in_string_utf8);
    return;
}

// Get the natural size required by the text using current width limit if enabled
VectorInt2 TextBlock::GetTextBounds() const
{
    return _implementation->GetTextBounds();
}

void TextBlock::SetTextContainerSize(
    const VectorInt2& in_size
    )
{
    _implementation->SetTextContainerSize(in_size);
    return;
}

void TextBlock::SetWidthLimit(
    const bool in_width_limit_enabled,
    const int in_width_limit
    )
{
    _implementation->SetWidthLimit(
        in_width_limit_enabled,
        in_width_limit
        );
    return;
}

void TextBlock::SetHorizontalLineAlignment(
    const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment
    )
{
    _implementation->SetHorizontalLineAlignment(
        in_horizontal_line_alignment
        );
    return;
}

void TextBlock::SetVerticalBlockAlignment(
    const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
    )
{
    _implementation->SetVerticalBlockAlignment(
        in_vertical_block_alignment
        );
    return;
}

void TextBlock::Update(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    _implementation->Update(
        in_draw_system, 
        in_command_list
        );
    return;
}

GeometryGeneric* const TextBlock::GetGeometry() const
{
    return _implementation->GetGeometry();
}
