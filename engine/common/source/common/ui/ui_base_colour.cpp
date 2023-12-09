#include "common/common_pch.h"
#include "common/ui/ui_base_colour.h"

const UIBaseColour UIBaseColour::FactoryDefault()
{
    return UIBaseColour();
}

const UIBaseColour UIBaseColour::FactoryRedBackground()
{
    return UIBaseColour(VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f));
}

const UIBaseColour UIBaseColour::FactoryGreenBackground()
{
    return UIBaseColour(VectorFloat4(0.0f, 1.0f, 0.0f, 1.0f));
}

const UIBaseColour UIBaseColour::FactoryBlueBackground()
{
    return UIBaseColour(VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f));
}

UIBaseColour::UIBaseColour(
    const VectorFloat4& in_clear_colour,
    const bool in_clear_background,
    const VectorFloat4& in_tint_colour,
    const bool in_draw_to_texture,
    const bool in_always_dirty,
    const float in_fade_start_seconds,
    const float in_fade_end_seconds
    )
    : _clear_colour(in_clear_colour)
    , _clear_background(in_clear_background)
    , _tint_colour(in_tint_colour)
    , _draw_to_texture(in_draw_to_texture)
    , _always_dirty(in_always_dirty)
    , _fade_start_seconds(in_fade_start_seconds)
    , _fade_end_seconds(in_fade_end_seconds)
{
    // Nop
}

const VectorFloat4 UIBaseColour::GetTintColour(
    const float in_time_accumulate_seconds,
    const VectorFloat4* const in_override_tint
    ) const
{
    //const float fade_start_time_seconds = in_create_index * _delay_per_create_index_seconds;
    //const float fade_end_time_seconds = fade_start_time_seconds + _fade_duration_seconds;

    if (_fade_end_seconds <= in_time_accumulate_seconds)
    {
        if (nullptr != in_override_tint)
        {
            return *in_override_tint;
        }
        return _tint_colour;
    }

    float alpha = 0.0f;
    if ((_fade_start_seconds <= in_time_accumulate_seconds) && (_fade_end_seconds != _fade_start_seconds))
    {
        alpha = (in_time_accumulate_seconds - _fade_start_seconds) / (_fade_end_seconds - _fade_start_seconds);
    }

    if (nullptr != in_override_tint)
    {
        return VectorFloat4(
            in_override_tint->GetX() * alpha,
            in_override_tint->GetY() * alpha,
            in_override_tint->GetZ() * alpha,
            in_override_tint->GetW() * alpha
            );
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

const bool UIBaseColour::GetTimeChangeDirty(const float in_pre_time, const float in_new_time) const
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

    if (_draw_to_texture != in_rhs._draw_to_texture)
    {
        return false;
    }
    if (_always_dirty != in_rhs._always_dirty)
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
