#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_scroll.h"

#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/log/log.h"
#include "common/ui/ui_data/ui_data_scroll.h"
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
        const VectorFloat2& in_value,
        const UIOrientation in_orientation
        )
    {
        switch(in_orientation)
        {
        default:
            break;
        case UIOrientation::THorizontal:
            in_out_layout.SetScrollHorizontal(in_value);
            break;
        case UIOrientation::TVertical:
            in_out_layout.SetScrollVertical(in_value);
            break;
        }

        return;
    }

    const VectorFloat2 CalculateTouchValue(
        const VectorFloat4& in_screen_pos, 
        const VectorFloat2& in_mouse_pos, 
        const UIOrientation in_orientation, 
        const VectorFloat2& in_range_low_high,
        const float in_length
        )
    {
        float ratio = 0.0f;
        switch(in_orientation)
        {
        default:
            break;
        case UIOrientation::THorizontal:
            {
                const float width = in_screen_pos[2] - in_screen_pos[0];
                ratio = width != 0.0f ? (in_mouse_pos.GetX() - in_screen_pos[0]) / width : 0.0f;
            }
            break;
        case UIOrientation::TVertical:
            {
                const float height = in_screen_pos[3] - in_screen_pos[1];
                ratio = height != 0.0f ? (in_mouse_pos.GetY() - in_screen_pos[1]) / height : 0.0f;
                ratio = 1.0f - ratio;
            }
            break;
        }
        VectorFloat2 result;
        result[0] = std::max(in_range_low_high[0], (in_range_low_high[0] + (ratio * (in_range_low_high[1] - in_range_low_high[0]))) - (in_length * 0.5f));
        result[1] = result[0] + in_length; //std::min(result[0] + in_length, in_range_low_high[1]);
        if (in_range_low_high[1] < result[1])
        {
            result[1] = in_range_low_high[1];
            result[0] = result[1] - in_length;
            // don't let result 0 go under the low range even when result 1 is clamped to top range, this may reduce length
            result[0] = std::max(in_range_low_high[0], result[0]);
        }
        return result;
    }

}

UIComponentScroll::UIComponentScroll(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const UIOrientation in_orientation
    )
    : _component_default(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    , _orientation(in_orientation)
    , _knot(nullptr) //_child_data_knot()
    , _value_change()
    , _value()
    , _range_low_high()
{
    //_child_data_knot = UIHierarchyNodeChildData::Factory();
}

UIComponentScroll::~UIComponentScroll()
{
    // Nop
}

const bool UIComponentScroll::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const UIOrientation in_orientation
    )
{
    bool dirty = false;
    if (true == _component_default.SetBase(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        ))
    {
        dirty = true;
    }
    if (_orientation != in_orientation)
    {
        _orientation = in_orientation;
        dirty = true;
    }
    return dirty;
}

const bool UIComponentScroll::Set(
    const std::function<void(const VectorFloat2&)>& in_value_change,
    const std::function<void(const VectorFloat2&)>& in_range_change,
    const VectorFloat2& in_value,
    const VectorFloat2& in_range_low_high
    )
{
    bool dirty = false;

    _value_change = in_value_change;
    _range_change = in_range_change;

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

const float UIComponentScroll::GetValueRatio() const
{
    const float length = _range_low_high[1] - _range_low_high[0];
    const float middle = (_value[0] + _value[1]) * 0.5f;
    const float inner_length = _value[1] - _value[0];
    const float result = 0.0f != length ? ((middle - _range_low_high[0] - (inner_length * 0.5f)) / (length - inner_length)) : 0.0f;
    return result;
}

const bool UIComponentScroll::SetStateFlag(const UIStateFlag in_state_flag)
{
    bool dirty = false;
    if (true == _component_default.SetStateFlag(in_state_flag))
    {
        dirty = true;
    }

    return dirty;
}

const bool UIComponentScroll::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentScroll::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentScroll::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentScroll::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

void UIComponentScroll::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
{
    _component_default.SetUVScrollManual(in_uv_scroll, in_manual_horizontal, in_manual_vertical);
    return;
}

void UIComponentScroll::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentScroll::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentScroll::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    UIDataScroll* const data = dynamic_cast<UIDataScroll*>(in_data);
    if (nullptr != data)
    {
        if (true == Set(
            data->GetOnValueChange(),
            data->GetOnRangeChange(),
            data->GetValue(),
            data->GetRangeLowHigh()
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

const bool UIComponentScroll::UpdateSize(
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
        const VectorFloat2 value(
            domain != 0.0f ? (_value[0] - _range_low_high[0]) / domain : 0.0f,
            domain != 0.0f ? (_value[1] - _range_low_high[0]) / domain : 0.0f
            );

        UILayout layout = _knot->GetLayout();
        TweakLayout(
            layout, 
            value,
            _orientation
            );

        _knot->SetLayoutOverride(layout);
    }

    bool dirty = false;
    if (true == _component_default.UpdateSize(
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
        ))
    {
        dirty = true;
    }
    return true;
}

void UIComponentScroll::GetDesiredSize(
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

const bool UIComponentScroll::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _component_default.PreDraw(
        in_draw_param,
        in_node
        );
}

void UIComponentScroll::OnInputTouch(
    const VectorFloat4& in_screen_pos,
    const VectorFloat2& in_mouse_pos
    )
{
    if(nullptr == _value_change)
    {
        return;
    }

    const VectorFloat2 value = CalculateTouchValue(in_screen_pos, in_mouse_pos, _orientation, _range_low_high, _value[1] - _value[0]);

    _value_change(value);

    //LOG_CONSOLE("OnInputTouch [%f %f]", value[0], value[1]);

    return;
}

void UIComponentScroll::OnInputClick(
    const VectorFloat4& in_screen_pos,
    const VectorFloat2& in_mouse_pos
    )
{
    if(nullptr == _value_change)
    {
        return;
    }

    const VectorFloat2 value = CalculateTouchValue(in_screen_pos, in_mouse_pos, _orientation, _range_low_high, _value[1] - _value[0]);

    _value_change(value);

    //LOG_CONSOLE("OnInputClick [%f %f]", value[0], value[1]);

    return;
}

const VectorFloat4 UIComponentScroll::GetTintColour() const
{
    return _component_default.GetTintColour();
}
