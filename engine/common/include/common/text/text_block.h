#pragma once

#include "common/text/text_enum.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class Shader;
class TextBlockImplementation;
class TextLocale;
class VectorInt2;
class TextFont;

class TextBlock
{
public:
    // Object is to be ready for use after contruction, so we create geometry with DrawSystem? allow for update however
    TextBlock(
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
        );

    ~TextBlock();

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds() const;

    void SetFont(
        TextFont& in_text_font,
        const int in_font_size
        );
    void SetText(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token
        );
    void SetTextContainerSize(
        const VectorInt2& in_size
        );
    void SetWidthLimit(
        const bool in_width_limit_enabled,
        const int in_width_limit
        );
    void SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
        );
    void SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        );

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
        );

    // return true if we needed to change geometry
    const bool Update(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );

    GeometryGeneric* const GetGeometry() const;

private:
    std::unique_ptr<TextBlockImplementation> _implementation;

};
