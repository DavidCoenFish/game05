#include "common/common_pch.h"
#include "common/ui/ui_base_colour.h"

UIBaseColour::UIBaseColour(
    const VectorFloat4& in_clear_colour,
    const bool in_clear_background,
    const VectorFloat4& in_tint_colour,
    //const float in_fade_duration_seconds,
    //const float in_delay_per_create_index_seconds
    const float in_fade_start_seconds,
    const float in_fade_end_seconds
    )
    : _clear_colour(in_clear_colour)
    , _clear_background(in_clear_background)
    , _tint_colour(in_tint_colour)
    //, _fade_duration_seconds(in_fade_duration_seconds)
    //, _delay_per_create_index_seconds(in_delay_per_create_index_seconds)
    , _fade_start_seconds(in_fade_start_seconds)
    , _fade_end_seconds(in_fade_end_seconds)
{
    // Nop
}

const VectorFloat4 UIBaseColour::GetTintColour(
    //const int in_create_index, 
    const float in_time_accumulate_seconds
    ) const
{
    //const float fade_start_time_seconds = in_create_index * _delay_per_create_index_seconds;
    //const float fade_end_time_seconds = fade_start_time_seconds + _fade_duration_seconds;

    if (_fade_end_seconds <= in_time_accumulate_seconds)
    {
        return _tint_colour;
    }

    float alpha = 0.0f;
    if ((_fade_start_seconds <= in_time_accumulate_seconds) && (_fade_end_seconds != _fade_start_seconds))
    {
        alpha = (in_time_accumulate_seconds - _fade_start_seconds) / (_fade_end_seconds - _fade_start_seconds);
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
    if (_fade_start_seconds != in_rhs._fade_start_seconds)
    {
        return false;
    }
    if (_fade_end_seconds != in_rhs._fade_end_seconds)
    {
        return false;
    }

    return true;
}

const bool UIBaseColour::operator!=(const UIBaseColour& in_rhs) const
{
    return !operator==(in_rhs);
}
