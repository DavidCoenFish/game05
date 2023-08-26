#pragma once

#include "common/text/text_enum.h"

class DrawSystem;
class TextBlock;
class TextFontImplementation;
class TextLocale;
class TextTexture;
class VectorInt2;
class VectorFloat2;
typedef struct FT_LibraryRec_* FT_Library;

class TextFont
{
public:
    TextFont(
        FT_Library in_library,
        const std::filesystem::path& in_font_file_path,
        TextTexture* const in_text_texture
        );
    ~TextFont();

    VectorInt2 CalculateTextBounds(
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const int in_font_size,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0,
        const int in_width_limit_new_line_height = 0
        );

    void GenerateGeometry(
        std::vector<uint8_t>& out_vertex_raw_data,
        const std::string& in_string_utf8,
        const TextLocale* const in_locale_token,
        const int in_font_size,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0,
        const int in_width_limit_new_line_height = 0,
        const TextEnum::HorizontalLineAlignment in_horizontal_line_alignment = TextEnum::HorizontalLineAlignment::Left,
        const TextEnum::VerticalBlockAlignment in_vertical_block_alignment = TextEnum::VerticalBlockAlignment::Top
        );

    void RestGlyphUsage();

private:
    std::unique_ptr<TextFontImplementation> _implementation;

};
