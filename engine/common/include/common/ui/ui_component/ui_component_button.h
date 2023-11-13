#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/i_ui_input.h"

class UIComponentButton : public IUIComponent, public IUIInput
{
    typedef IUIComponent TSuper;
public:
    typedef std::function<void(const VectorFloat2&)> TOnValueChange;
    typedef std::function<const std::string()> TGetTooltip;

    UIComponentButton(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const std::function<void(const VectorFloat2&)>& in_on_click = nullptr,
        const bool in_allow_repeat = false
        );
    virtual ~UIComponentButton();

    const bool Set(
        const TOnValueChange& in_on_click,
        const TGetTooltip& in_get_tooltip,
        const bool in_allow_repeat
        );

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void OnInputClick(
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
    TOnValueChange _on_click;
    TGetTooltip _get_tooltip;
    bool _allow_repeat;

};
