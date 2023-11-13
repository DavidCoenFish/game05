#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/i_ui_input.h"

class UIComponentButton : public IUIComponent, public IUIInput
{
    typedef IUIComponent TSuper;
public:
    UIComponentButton(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const std::function<void(const VectorFloat2&)>& in_on_click = nullptr,
        const bool in_allow_repeat = false
        );
    virtual ~UIComponentButton();

    const bool Set(
        const std::function<void(const VectorFloat2&)>& in_on_click,
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

private:
    std::function<void(const VectorFloat2&)> _on_click;
    bool _allow_repeat;

};
