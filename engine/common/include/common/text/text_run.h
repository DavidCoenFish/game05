#pragma once

#include "common/text/text_enum.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class Shader;
class TextRunImplementation;
class TextLocale;
class VectorInt2;
class TextFont;
class IconFont;

struct TextRunData
{
    explicit TextRunData(
        const int _icon_id = 0, // Zero if you are using a string and not an icon
        IconFont* const in_icon_font = nullptr,
        const std::string& in_string_utf8 = std::string(""),
        TextLocale* const in_locale_token = nullptr,
        TextFont* const in_text_font = nullptr,
        const int in_font_size = 0,
        const int in_new_line_height = 0,
        const VectorFloat4& in_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    //const bool operator==(const TextRunData& in_rhs) const;

    // if not zero, we are drawing an icon
    int _icon_id;

    std::string _string_utf8;
    TextLocale* const _locale_token;
    TextFont* const _text_font;
    int _font_size;
    VectorFloat4 _colour;

    int _new_line_height;
};

class TextRun
{
public:
    // Do we keep the defaults, and each run has the settings? or simplify so we just have global settings at top, and data for each run
    TextRun(
        const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment = TextEnum::HorizontalLineAlignment::Left,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment = TextEnum::VerticalBlockAlignment::Top
        );

    ~TextRun();

        //const std::string& in_string_utf8,
        //const TextLocale* const in_locale_token,

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds();

    const bool SetTextRunArray(
        const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array
        );
    const bool SetTextContainerSize(
        const VectorInt2& in_size
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

    const bool Set(
        const std::vector<std::shared_ptr<TextRunData>>& in_text_run_array,
        const VectorInt2& in_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment
        );

    // Rather than a seperate Update function, that could be forgoten, have the update in the getter, results in out_geometry_dirty
    GeometryGeneric* const GetGeometry(
        bool& out_geometry_dirty,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );

private:
    std::unique_ptr<TextRunImplementation> _implementation;

};
