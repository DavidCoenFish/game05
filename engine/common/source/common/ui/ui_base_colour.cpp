#include "common/common_pch.h"
#include "common/ui/ui_base_colour.h"

const UIBaseColour UIBaseColour::FactoryDefault()
{
    return UIBaseColour();
}

const UIBaseColour UIBaseColour::FactoryGreyBackground()
{
    return UIBaseColour(VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f));
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
    const bool in_draw_to_texture,
    const bool in_always_dirty
    )
    : _clear_colour(in_clear_colour)
    , _clear_background(in_clear_background)
    , _draw_to_texture(in_draw_to_texture)
    , _always_dirty(in_always_dirty)
{
    // Nop
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

    if (_draw_to_texture != in_rhs._draw_to_texture)
    {
        return false;
    }
    if (_always_dirty != in_rhs._always_dirty)
    {
        return false;
    }

    return true;
}

const bool UIBaseColour::operator!=(const UIBaseColour& in_rhs) const
{
    return !operator==(in_rhs);
}
