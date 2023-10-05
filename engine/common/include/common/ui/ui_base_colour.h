#pragma once

#include "common/math/vector_float4.h"

class UIBaseColour
{
public:
    explicit UIBaseColour(
        const VectorFloat4& in_clear_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
        const bool in_clear_background = true,
        const VectorFloat4& in_tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
        const float in_fade_start_time_seconds = 0.0f,
        const float in_fade_end_time_seconds = 0.0f
        );

private:
    VectorFloat4 _clear_colour;
    bool _clear_background;
    VectorFloat4 _tint_colour;
    float _fade_start_time_seconds;
    float _fade_end_time_seconds;

};

