#include "common/common_pch.h"
#include "common/ui/ui_tint_colour.h"


const UITintColour UITintColour::FactoryDefault()
{
    return UITintColour();
}

const UITintColour UITintColour::FactoryBlack()
{
    return UITintColour(
        VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
        );
}

const UITintColour UITintColour::FactoryFade(
    const float in_fade_duration_seconds,
    const float in_delay_per_create_index_seconds,
    const int in_creation_index
    )
{
    const float fade_start_seconds = in_delay_per_create_index_seconds * in_creation_index;
    const float fade_end_seconds = fade_start_seconds + in_fade_duration_seconds;
    return UITintColour(
        VectorFloat4::s_white,
        fade_start_seconds,
        fade_end_seconds
        );
}

UITintColour::UITintColour(
    const VectorFloat4& in_tint_colour,
    const float in_fade_start_seconds,
    const float in_fade_end_seconds
    )
    : _tint_colour(in_tint_colour)
    , _fade_start_seconds(in_fade_start_seconds)
    , _fade_end_seconds(in_fade_end_seconds)
{
    // Nop
}

const bool UITintColour::GetTimeChangeDirty(const float in_pre_time, const float in_new_time) const
{
    /// only ensure no state change over previous to new time if both are before the range of the fade, or both after
    if ((_fade_end_seconds <= in_pre_time) && (_fade_end_seconds <= in_new_time))
    {
        return false;
    }
    if ((in_pre_time <= _fade_start_seconds) && (in_new_time <= _fade_start_seconds))
    {
        return false;
    }

    return true;
}

const VectorFloat4 UITintColour::GetTintColour(
    const float in_time_accumulate_seconds
    ) const
{
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

const bool UITintColour::operator==(const UITintColour& in_rhs) const
{
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

const bool UITintColour::operator!=(const UITintColour& in_rhs) const
{
    return !operator==(in_rhs);
}
