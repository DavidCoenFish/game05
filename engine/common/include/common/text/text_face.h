#pragma once

#include "common/text/text_enum.h"

class DrawSystem;
class TextBlock;
class TextFaceImplementation;
class VectorInt2;
typedef struct FT_LibraryRec_* FT_Library;

class TextFace
{
public:
    TextFace(
        FT_Library in_library,
        const std::filesystem::path& in_font_file_path
        );
    ~TextFace();

    std::shared_ptr<TextBlock> MakeBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::string& in_string_utf8,
        const int in_char_size,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment = TextEnum::HorizontalLineAlignment::Enum::Left,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment = TextEnum::VerticalBlockAlignment::Enum::Top
        );

    VectorInt2 CalculateTextBounds(
        const std::string& in_string_utf8,
        const int in_char_size,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0
        );

    void RestGlyphUsage();

private:
    std::unique_ptr<TextFaceImplementation> _implementation;

};
