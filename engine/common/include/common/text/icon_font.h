#pragma once

#include "common/math/vector_float4.h"

class DrawSystem;
class IconFontImplementation;
class TextPreVertex;
class TextTexture;
class VectorInt2;
class VectorFloat2;

class IconFont
{
public:
    IconFont(
        TextTexture* const in_text_texture
        );
    ~IconFont();

    void AddIcon(
        const int in_icon_id,
        const int in_width,
        const int in_height,
        const unsigned char* const in_buffer
        );

    void CalculateTextBounds(
        TextPreVertex& text_pre_vertex,
        VectorFloat2& in_out_cursor, // allow multiple fonts to append pre vertex data
        const int in_icon_id,
        const int in_new_line_height = 0,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0
        );

    void RestIconUsage();

private:
    std::unique_ptr<IconFontImplementation> _implementation;

};
