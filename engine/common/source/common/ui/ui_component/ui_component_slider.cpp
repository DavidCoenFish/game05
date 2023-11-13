#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_slider.h"

#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/ui/ui_data/ui_data_slider.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_enum.h"

namespace
{
    void TweakLayout(
        UILayout& in_out_layout,
        const float in_value,
        const UIOrientation in_orientation
        )
    {
        switch(in_orientation)
        {
        default:
            break;
        case UIOrientation::THorizontal:
            in_out_layout.SetSliderHorizontal(in_value);
            break;
        case UIOrientation::TVertical:
            in_out_layout.SetSliderVertical(in_value);
            break;
        }
        return;
    }

    const float CalculateTouchValue(
        const VectorFloat4& in_screen_pos, 
        const VectorFloat2& in_mouse_pos, 
        const UIOrientation in_orientation, 
        const VectorFloat2& in_range_low_high
        )
    {
        float value = 0.0f;
        switch(in_orientation)
        {
        default:
            break;
        case UIOrientation::THorizontal:
            {
                const float width = in_screen_pos[2] - in_screen_pos[0];
                value = width != 0.0f ? (in_mouse_pos.GetX() - in_screen_pos[0]) / width : 0.0f;
            }
            break;
        case UIOrientation::TVertical:
            {
                const float height = in_screen_pos[3] - in_screen_pos[1];
                value = height != 0.0f ? (in_mouse_pos.GetY() - in_screen_pos[1]) / height : 0.0f;
            }
            break;
        }
        value = (in_range_low_high[0] + (value * (in_range_low_high[1] - in_range_low_high[0])));
        return value;
    }
}

UIComponentSlider::UIComponentSlider(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const UIOrientation in_orientation
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    , _orientation(in_orientation)
    , _knot(nullptr) //_child_data_knot()
    , _value_change()
    , _value(0.0f)
    , _range_low_high()
{
    //_child_data_knot = UIHierarchyNodeChildData::Factory();
}

UIComponentSlider::~UIComponentSlider()
{
    // Nop
}

const bool UIComponentSlider::SetModelOther(
    const UIOrientation in_orientation
    )
{
    bool dirty = false;
    if (_orientation != in_orientation)
    {
        _orientation = in_orientation;
        dirty = true;
    }
    return dirty;
}

const bool UIComponentSlider::Set(
    const std::function<void(const float)>& in_value_change,
    const std::function<const std::string()>& in_get_tooltip,
    const float in_value,
    const VectorFloat2& in_range_low_high
    )
{
    bool dirty = false;

    _value_change = in_value_change;
    _get_tooltip = in_get_tooltip;

    if (_value != in_value)
    {
        _value = in_value;
        dirty = true;
    }

    if (_range_low_high != in_range_low_high)
    {
        _range_low_high = in_range_low_high;
        dirty = true;
    }

    return dirty;
}

const bool UIComponentSlider::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    UIDataSlider* const data = dynamic_cast<UIDataSlider*>(in_data);
    if (nullptr != data)
    {
        if (true == Set(
            data->GetOnValueChange(),
            data->GetTooltip(),
            data->GetValue(),
            data->GetRangeLowHigh()
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

    _knot = nullptr;
    UIData* const knot_data = data ? data->GetKnotChildData() : nullptr;
    in_out_child_data.VisitComponents([this, knot_data](IUIComponent* const in_component, UIHierarchyNode* const){
            if (knot_data == in_component->GetSourceToken())
            {
                _knot = in_component;
                return false;
            }
            return true;
        });

    return dirty;
}

const bool UIComponentSlider::UpdateSize(
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
    if (nullptr != _knot)
    {
        const float domain = _range_low_high[1] - _range_low_high[0];
        const float value = domain != 0.0f ? (_value - _range_low_high[0]) / domain : 0.0f;
        UILayout layout = _knot->GetLayout();
        TweakLayout(
            layout, 
            value,
            _orientation
            );
        _knot->SetLayoutOverride(layout);
    }
    return TSuper::UpdateSize(
        in_draw_system,
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

void UIComponentSlider::OnInputTouch(
    const VectorFloat4& in_screen_pos,
    const VectorFloat2& in_mouse_pos,
    std::string& out_tooltip
    )
{
    if(nullptr == _value_change)
    {
        return;
    }

    const float value = CalculateTouchValue(in_screen_pos, in_mouse_pos, _orientation, _range_low_high);

    _value_change(value);

    if (nullptr != _get_tooltip)
    {
        out_tooltip = _get_tooltip();
    }

    return;
}

void UIComponentSlider::OnInputClick(
    const VectorFloat4& in_screen_pos,
    const VectorFloat2& in_mouse_pos
    )
{
    if(nullptr == _value_change)
    {
        return;
    }

    const float value = CalculateTouchValue(in_screen_pos, in_mouse_pos, _orientation, _range_low_high);

    _value_change(value);

    return;
}

void UIComponentSlider::OnHover(
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

