#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_slider.h"

#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/ui/ui_data/ui_data_float.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_shader_enum.h"
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
    const UIOrientation in_orientation
    )
    : _component_default(
        in_base_colour,
        in_layout
        )
    , _orientation(in_orientation)
    , _child_data_knot()
    , _value_change()
    , _value(0.0f)
    , _range_low_high()
{
    _child_data_knot = UIHierarchyNodeChildData::Factory();
}

UIComponentSlider::~UIComponentSlider()
{
    // Nop
}

const bool UIComponentSlider::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const UIOrientation in_orientation
    )
{
    bool dirty = false;
    if (true == _component_default.SetBase(
        in_base_colour,
        in_layout
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

const bool UIComponentSlider::Set(
    const std::function<void(const float)>& in_value_change,
    const float in_value,
    const VectorFloat2& in_range_low_high
    )
{
    bool dirty = false;

    _value_change = in_value_change;

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

const bool UIComponentSlider::SetStateFlag(const UIStateFlag in_state_flag)
{
    bool dirty = false;
    if (true == _component_default.SetStateFlag(in_state_flag))
    {
        dirty = true;
    }

    if (true == UIHierarchyNodeChildData::RecurseSetStateFlagInput(_child_data_knot.get(), in_state_flag))
    {
        dirty = true;
    }

    return dirty;
}

const UIStateFlag UIComponentSlider::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentSlider::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentSlider::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentSlider::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentSlider::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    UIDataFloat* const data = dynamic_cast<UIDataFloat*>(in_data);
    if (nullptr != data)
    {
        if (true == Set(
            data->GetOnValueChange(),
            data->GetValue(),
            data->GetRangeLowHigh()
            ))
        {
            dirty = true;
        }
    }

    UIHierarchyNodeChildData* knot = _child_data_knot.get();
    UIData* const knot_data = data ? data->GetKnotChildData() : nullptr;

    if (nullptr != knot)
    {
        if (true == knot->ApplyFactory(knot_data, in_param, 0))
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

void UIComponentSlider::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_extra_data,
    UILayout* const in_layout_override
    )
{
    _component_default.UpdateSize(
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
        out_screen_space,
        in_layout_override
        );

    if (nullptr != _child_data_knot->_component)
    {
        const float domain = _range_low_high[1] - _range_low_high[0];
        const float value = domain != 0.0f ? (_value - _range_low_high[0]) / domain : 0.0f;
        UILayout layout = _child_data_knot->_component->GetLayout();
        TweakLayout(
            layout, 
            value,
            _orientation
            );

        const auto texture_size = in_out_node.GetTextureSize(in_draw_system);
        _child_data_knot->_component->UpdateSize(
            in_draw_system,
            texture_size, //in_parent_window,//in_parent_size,
            VectorInt2(),//in_parent_offset,
            texture_size, //in_parent_window,
            in_ui_scale,
            in_time_delta, 
            *_child_data_knot->_geometry, 
            *_child_data_knot->_node,
            in_parent_screen_space,
            *_child_data_knot->_screen_space,
            in_extra_data,
            &layout
            );
    }

    return;
}

void UIComponentSlider::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    UILayout* const in_layout_override
    )
{
    return _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        in_layout_override
        );
}

const bool UIComponentSlider::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    bool dirty = false;

    if ((nullptr != _child_data_knot) && (nullptr != _child_data_knot->_component))
    {
        if (true == _child_data_knot->_component->PreDraw(
            in_draw_param,
            *(_child_data_knot->_node.get())
            ))
        {
            dirty = true;
        }
    }

    if (true == in_node.PreDraw(
        in_draw_param
        ))
    {
        dirty = true;
    }
    if (true == in_node.Draw(
        in_draw_param,
        dirty
        ))
    {
        if (nullptr != _child_data_knot)
        {
            _child_data_knot->Draw(in_draw_param);
        }

        dirty = true;
    }

    return dirty;
}

void UIComponentSlider::OnInputTouch(
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
