#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_button.h"
#include "common/ui/ui_data/ui_data_button.h"
#include "common/ui/ui_data/ui_data_toggle.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

namespace
{
    const int CalculateRepeatCount(const float in_time_seconds)
    {
        if (in_time_seconds < 0.5f)
        {
            return 0;
        }
        const float temp = in_time_seconds + 0.5f;
        const int repeat_count = static_cast<int>(temp * temp);
        return repeat_count;
    }
}

UIComponentButton::UIComponentButton(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const std::function<void(const VectorFloat2&)>& in_on_click,
    const bool in_allow_repeat
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    , _on_click(in_on_click)
    , _allow_repeat(in_allow_repeat)
{
    // Nop
}

UIComponentButton::~UIComponentButton()
{
    // Nop
}

const bool UIComponentButton::Set(
    const TOnValueChange& in_on_click,
    const TGetTooltip& in_get_tooltip,
    const bool in_allow_repeat
    )
{
    _on_click = in_on_click;
    _get_tooltip = in_get_tooltip;
    _allow_repeat = in_allow_repeat;
    return false;
}

const bool UIComponentButton::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    const UIDataButton* const data = dynamic_cast<const UIDataButton*>(in_data);
    if (nullptr != data)
    {
        if (true == Set(
            data->GetOnClick(),
            data->GetTooltip(),
            data->GetRepeat()
            ))
        {
            dirty = true;
        }
    }

    const UIDataToggle* const data_toggle = dynamic_cast<const UIDataToggle*>(in_data);
    if (nullptr != data_toggle)
    {
        if (true == Set(
            data_toggle->GetOnClick(),
            data_toggle->GetTooltip(),
            false
            ))
        {
            dirty = true;
        }
    }

    if (true == TSuper::UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        ))
    {
        dirty = true;
    }

    return dirty;
}

void UIComponentButton::OnInputClick(
    UIRootInputState&,
    const VectorFloat4&, // in_screen_pos,
    const VectorFloat2& in_mouse_pos
    )
{
    if (nullptr != _on_click)
    {
        _on_click(in_mouse_pos);
    }
    return;
}

void UIComponentButton::OnInputRepeat(
    const VectorFloat4&,
    const VectorFloat2& in_mouse_pos,
    const float in_duration,
    const float in_last_timestep,
    std::string& out_tooltip
    )
{
    if ((false == _allow_repeat) ||
        (nullptr == _on_click))
    {
        return;
    }

    const int count_before = CalculateRepeatCount(in_duration - in_last_timestep);
    const int count_after = CalculateRepeatCount(in_duration);
    if (count_before != count_after)
    {
        _on_click(in_mouse_pos);
    }

    if (nullptr != _get_tooltip)
    {
        out_tooltip = _get_tooltip();
    }

    return;
}

void UIComponentButton::OnHover(
    const VectorFloat4&,
    const VectorFloat2&,
    std::string& out_tooltip
    )
{
    if (nullptr != _get_tooltip)
    {
        out_tooltip = _get_tooltip();
    }

    return;
}

