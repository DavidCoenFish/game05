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

class TextBlockImplementation
{
public:
    TextBlockImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        TextFont& in_text_font,
        const int in_font_size,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
        : _calculate_dirty(true)
        , _geometry_dirty(true)
        , _geometry()
        , _text_font(&in_text_font)
        , _font_size(in_font_size)
        , _string_utf8(in_string_utf8)
        , _locale_token(in_locale_token)
        , _container_size(in_containter_size)
        , _width_limit_enabled(in_width_limit_enabled)
        , _width_limit(in_width_limit)
        , _horizontal_line_alignment(in_horizontal_line_alignment)
        , _vertical_block_alignment(in_vertical_block_alignment)
    {
        Update(
            in_draw_system,
            in_command_list
            );
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds()
    {
        if (true == _calculate_dirty)
        {
            _calculate_dirty = false;
            _text_bounds = _text_font->CalculateTextBounds(
                _string_utf8,
                _locale_token,
                _font_size,
                _width_limit_enabled,
                _width_limit
                );
        }

        return _text_bounds;
    }

    void SetFont(
        TextFont& in_text_font,
        const int in_font_size
        )
    {
        if ((_font_size != in_font_size) ||
            (_text_font != &in_text_font))
        {
            _calculate_dirty = true;
            _geometry_dirty = true;
            _font_size = in_font_size;
            _text_font = &in_text_font;
        }
        return;
    }

    void SetText(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token
        )
    {
        if ((_string_utf8 != in_string_utf8) ||
            (_locale_token != in_locale_token))
        {
            _calculate_dirty = true;
            _geometry_dirty = true;
            _string_utf8 = in_string_utf8;
            _locale_token = in_locale_token;
        }
        return;
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
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return;
    }

    void SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
        )
    {
        if (_horizontal_line_alignment != in_horizontal_line_alignment)
        {
            _horizontal_line_alignment = in_horizontal_line_alignment;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return;
    }

    void SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
    {
        if (_vertical_block_alignment != in_vertical_block_alignment)
        {
            _vertical_block_alignment = in_vertical_block_alignment;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return;
    }


    void Set(
        TextFont& in_text_font,
        const int in_font_size,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const VectorInt2& in_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        )
    {
        SetFont(
            in_text_font,
            in_font_size
            );
        SetText(
            in_string_utf8,
            in_locale_token
            );
        SetTextContainerSize(
            in_size
            );
        SetWidthLimit(
            in_width_limit_enabled,
            in_width_limit
            );
        SetHorizontalLineAlignment(
            in_horizontal_line_alignment
            );
        SetVerticalBlockAlignment(
            in_vertical_block_alignment
            );
        return;
    }

    const bool Update(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        )
    {
        if (false == _geometry_dirty)
        {
            return false;
        }
        _geometry_dirty = false;

        std::vector<uint8_t> vertex_raw_data;
        _text_font->GenerateGeometry(
            vertex_raw_data,
            _string_utf8,
            _locale_token,
            _font_size,
            _container_size,
            _width_limit_enabled,
            _width_limit,
            _font_size, // new line height
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
                8
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

        return true;
    }

    GeometryGeneric* const GetGeometry() const
    {
        return _geometry.get();
    }

private:
    bool _calculate_dirty;
    bool _geometry_dirty;
    std::shared_ptr<GeometryGeneric> _geometry;
    TextFont* _text_font;
    int _font_size;
    std::string _string_utf8;
    const TextLocale* _locale_token;
    VectorInt2 _text_bounds;
    VectorInt2 _container_size;
    bool _width_limit_enabled;
    int _width_limit;
    TextEnum::HorizontalLineAlignment _horizontal_line_alignment;
    TextEnum::VerticalBlockAlignment _vertical_block_alignment;

};

TextBlock::TextBlock(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    TextFont& in_text_font,
    const int in_font_size,
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    _implementation = std::make_unique<TextBlockImplementation>(
        in_draw_system,
        in_command_list,
        in_text_font,
        in_font_size,
        in_string_utf8,
        in_locale_token,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
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

void TextBlock::SetFont(
    TextFont& in_text_font,
    const int in_font_size
    )
{
    _implementation->SetFont(
        in_text_font,
        in_font_size
        );
}

void TextBlock::SetText(
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token
    )
{
    _implementation->SetText(
        in_string_utf8,
        in_locale_token
        );
}

void TextBlock::SetTextContainerSize(
    const VectorInt2& in_size
    )
{
    _implementation->SetTextContainerSize(
        in_size
        );
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
}

void TextBlock::SetHorizontalLineAlignment(
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
    )
{
    _implementation->SetHorizontalLineAlignment(
        in_horizontal_line_alignment
        );
}

void TextBlock::SetVerticalBlockAlignment(
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    _implementation->SetVerticalBlockAlignment(
        in_vertical_block_alignment
        );
}

void TextBlock::Set(
    TextFont& in_text_font,
    const int in_font_size,
    const std::string& in_string_utf8,
    const TextLocale* const in_locale_token,
    const VectorInt2& in_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
    )
{
    _implementation->Set(
        in_text_font,
        in_font_size,
        in_string_utf8,
        in_locale_token,
        in_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
        );
}

const bool TextBlock::Update(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    return _implementation->Update(
        in_draw_system, 
        in_command_list
        );
}

GeometryGeneric* const TextBlock::GetGeometry() const
{
    return _implementation->GetGeometry();
}
