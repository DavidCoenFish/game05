#pragma once

#include "common/text/text_enum.h"
#include "common/math/vector_float4.h"

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
    // Object is to be ready for use after contruction? so we create geometry with DrawSystem? allow for update however
    // one main use case is ui, which may not have the container size ready? or change ui to not creat a text block till ready
    // Move to lazy init of geometry as ui use case will not know the container size at construction
    TextBlock(
        TextFont& in_text_font,
        const int in_font_size,
        const float in_new_line_gap_ratio, // new line = in_font_size + (in_font_size * in_new_line_gap_ratio)
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment = TextEnum::HorizontalLineAlignment::Left,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment = TextEnum::VerticalBlockAlignment::Top,
        const VectorFloat4& in_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
        const float in_ui_scale = 1.0f
        );

    ~TextBlock();

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds();

    TextFont* const GetFont() const;
    const bool SetFont(
        TextFont& in_text_font
        );
    const bool SetFontSize(
        const int in_font_size
        );
    const bool SetNewLineGapRatio(
        const float in_new_line_gap_ratio // new line = in_font_size + (in_font_size * in_new_line_gap_ratio)
        );
    const bool SetText(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token
        );
    const bool SetTextContainerSize(
        const VectorInt2& in_size
        );
    const bool GetWidthLimitEnabled() const;
    const bool SetWidthLimitEnabled(
        const bool in_width_limit_enabled
        );
    const bool SetWidthLimit(
        const bool in_width_limit_enabled,
        const int in_width_limit
        );
    const bool SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment
        );
    const bool SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        );
    const bool SetColour(
        const VectorFloat4& in_colour
        );
    const bool SetUIScale(
        const float in_ui_scale
        );

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
        );

    // Rather than a seperate Update function, that could be forgoten, have the update in the getter, results in out_geometry_dirty
    GeometryGeneric* const GetGeometry(
        bool& out_geometry_dirty,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );

private:
    std::unique_ptr<TextBlockImplementation> _implementation;

};
