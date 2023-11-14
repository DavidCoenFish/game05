#pragma once

class UIRootInputState;
class VectorFloat2;
class VectorFloat4;


/// Interface class for elments interacting with input.
/// moving towards treating mouse input as anoter touch.
/// may mean an extra touch is added when right mouse button is down, and a persitant touch for left button up or down
class IUIInput
{
public:
    virtual ~IUIInput();

    /// treat mouse as just another touch, and touch as finger pressing/touching screen, or mouse click held down
    /// output a string for the tooltip, if there is any
    virtual void OnInputTouch(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_touch_pos,
        std::string& out_tooltip
        );

    /// A click is when the touch is released over the same element the touch started on, ie, allow drag off and release as cancel
    virtual void OnInputClick(
        UIRootInputState& in_input_state,
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_touch_pos
        );

    /// allow slider buttons to step value when button is held, holding touch on element can repeat an action
    /// output a string for the tooltip, if there is any
    virtual void OnInputRepeat(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_touch_pos,
        const float in_duration,
        const float in_last_timestep,
        std::string& out_tooltip
        );

    /// output a string for the tooltip, if there is any
    virtual void OnHover(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_touch_pos,
        std::string& out_tooltip
        );

};
