#include "common/common_pch.h"
#include "common/ui/ui_base_colour.h"

UIBaseColour::UIBaseColour(
    const VectorFloat4& in_clear_colour,
    const bool in_clear_background,
    const VectorFloat4& in_tint_colour,
    const float in_fade_start_time_seconds,
    const float in_fade_end_time_seconds
    )
    : _clear_colour(in_clear_colour)
    , _clear_background(in_clear_background)
    , _tint_colour(in_tint_colour)
    , _fade_start_time_seconds(in_fade_start_time_seconds)
    , _fade_end_time_seconds(in_fade_end_time_seconds)
{
    // Nop
}
