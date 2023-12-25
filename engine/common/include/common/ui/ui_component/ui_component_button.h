#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/i_ui_input.h"

class UIComponentButton : public IUIComponent, public IUIInput
{
    typedef IUIComponent TSuper;
public:
    typedef std::function<void(const VectorFloat2&)> TOnClick;
    typedef std::function<const std::string()> TGetTooltip;

    UIComponentButton(
        const TOnClick& in_on_click,
        const TGetTooltip& in_get_tooltip,
        const bool in_allow_repeat
        );
    virtual ~UIComponentButton();

    const bool Set(
        const TOnClick& in_on_click,
        const TGetTooltip& in_get_tooltip,
        const bool in_allow_repeat
        );

private:
    virtual void OnInputClick(
        UIRootInputState& in_input_state,
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos
        ) override;

    virtual void OnInputRepeat(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos,
        const float in_duration,
        const float in_last_timestep,
        std::string& out_tooltip
        ) override;

    virtual void OnHover(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_touch_pos,
        std::string& out_tooltip
        ) override;

private:
    TOnClick _on_click;
    TGetTooltip _get_tooltip;
    bool _allow_repeat;

};
