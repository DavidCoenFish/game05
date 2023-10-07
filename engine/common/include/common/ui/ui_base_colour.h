#pragma once

#include "common/math/vector_float4.h"

class UIBaseColour
{
public:
    explicit UIBaseColour(
        const VectorFloat4& in_clear_colour = VectorFloat4(0.0f, 0.0f, 0.0f, 0.0f),
        const bool in_clear_background = true,
        const VectorFloat4& in_tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f),
        const float in_fade_duration_seconds = 0.0f,
        const float in_delay_per_create_index_seconds = 0.0f
        );

    const VectorFloat4 GetTintColour(const int in_create_index, const float in_time_accumulate_seconds) const;

    const VectorFloat4& GetClearColourRef() const { return _clear_colour; }
    const bool GetClearBackground() const { return _clear_background; }

    const bool operator==(const UIBaseColour& in_rhs) const;
    const bool operator!=(const UIBaseColour& in_rhs) const;

private:
    VectorFloat4 _clear_colour;
    bool _clear_background;
    VectorFloat4 _tint_colour;
    float _fade_duration_seconds;
    float _delay_per_create_index_seconds;

};

