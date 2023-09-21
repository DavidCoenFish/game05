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

    void BuildPreVertexData(
        TextPreVertex& in_out_text_pre_vertex,
        VectorInt2& in_out_cursor, // Allow multiple fonts to append pre vertex data
        const int in_icon_id,
        const float in_new_line_gap_ratio = 0.0f,
        const bool in_width_limit_enabled = false,
        const int in_width_limit = 0,
        const float in_ui_scale = 1.0f,
        const VectorFloat4 in_colour_tint = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f)
        );

    void RestIconUsage();

private:
    std::unique_ptr<IconFontImplementation> _implementation;

};
