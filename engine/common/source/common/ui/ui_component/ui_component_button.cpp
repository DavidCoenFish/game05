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
    const std::function<void(const VectorFloat2&)>& in_on_click,
    const bool in_allow_repeat
    )
    : _component_default(
        in_base_colour,
        in_layout
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

const bool UIComponentButton::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
{
    return _component_default.SetBase(
        in_base_colour,
        in_layout
        );
}

const bool UIComponentButton::Set(
    const std::function<void(const VectorFloat2&)>& in_on_click,
    const bool in_allow_repeat
    )
{
    _on_click = in_on_click;
    _allow_repeat = in_allow_repeat;
    return false;
}

const bool UIComponentButton::SetStateFlag(const UIStateFlag in_state_flag)
{
    return _component_default.SetStateFlag(in_state_flag);
}

const bool UIComponentButton::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentButton::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentButton::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentButton::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

void UIComponentButton::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentButton::GetSourceToken() const
{
    return _component_default.GetSourceToken();
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
            false //data->GetRepeat()
            ))
        {
            dirty = true;
        }
    }

    if (true == _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        ))
    {
        dirty = true;
    }

    return dirty;
}

const bool UIComponentButton::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space
    )
{
    return _component_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node,
        in_parent_screen_space,
        out_screen_space
        );
}

void UIComponentButton::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    return _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );
}

const bool UIComponentButton::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _component_default.PreDraw(
        in_draw_param,
        in_node
        );
}

void UIComponentButton::OnInputClick(
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
    const float in_last_timestep
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

}


