#include "common/common_pch.h"
#include "common/ui/ui_base_colour.h"

UIBaseColour::UIBaseColour(
    const VectorFloat4& in_clear_colour,
    const bool in_clear_background,
    const VectorFloat4& in_tint_colour,
    const float in_fade_duration_seconds,
    const float in_delay_per_create_index_seconds
    )
    : _clear_colour(in_clear_colour)
    , _clear_background(in_clear_background)
    , _tint_colour(in_tint_colour)
    , _fade_duration_seconds(in_fade_duration_seconds)
    , _delay_per_create_index_seconds(in_delay_per_create_index_seconds)
{
    // Nop
}

const VectorFloat4 UIBaseColour::GetTintColour(const int in_create_index, const float in_time_accumulate_seconds) const
{
    const float fade_start_time_seconds = in_create_index * _delay_per_create_index_seconds;
    const float fade_end_time_seconds = fade_start_time_seconds + _fade_duration_seconds;

    if (fade_end_time_seconds <= in_time_accumulate_seconds)
    {
        return _tint_colour;
    }

    float alpha = 0.0f;
    if ((fade_start_time_seconds <= in_time_accumulate_seconds) && (0.0f != _fade_duration_seconds))
    {
        alpha = (in_time_accumulate_seconds - fade_start_time_seconds) / (_fade_duration_seconds);
    }

    // premultiplied alpha
    const VectorFloat4 result(
        _tint_colour[0] * alpha,
        _tint_colour[1] * alpha,
        _tint_colour[2] * alpha,
        _tint_colour[3] * alpha
        );

    return result;
}

const bool UIBaseColour::operator==(const UIBaseColour& in_rhs) const
{
    if (_clear_colour != in_rhs._clear_colour)
    {
        return false;
    }
    if (_clear_background != in_rhs._clear_background)
    {
        return false;
    }
    if (_tint_colour != in_rhs._tint_colour)
    {
        return false;
    }
    if (_fade_duration_seconds != in_rhs._fade_duration_seconds)
    {
        return false;
    }
    if (_delay_per_create_index_seconds != in_rhs._delay_per_create_index_seconds)
    {
        return false;
    }

    return true;
}

const bool UIBaseColour::operator!=(const UIBaseColour& in_rhs) const
{
    return !operator==(in_rhs);
}
