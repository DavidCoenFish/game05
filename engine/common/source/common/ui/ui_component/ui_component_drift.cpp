#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_drift.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_screen_space.h"

namespace
{
    const float SmoothStep(const float in_value)
    {
        //3x^2 - 2x^3
        const float v_v = in_value * in_value;
        const float result = (3.0f * v_v) - (2.0f * v_v * in_value);
        return result;
    }

    const float CalculateRatio(
        const float in_duration, 
        const float in_time_accumulate_seconds
        )
    {
        if (in_duration <= 0.0f)
        {
            return 1.0f;
        }
        if (in_duration < in_time_accumulate_seconds)
        {
            return 1.0f;
        }

        const float time = std::max(0.0f, in_time_accumulate_seconds);
        // using the top half of smooth step, ie, only ease out, no ease in
        // [0 ... 1] =>  [0.5 ... 1.0]
        const float temp = (in_time_accumulate_seconds + in_duration) / (in_duration + in_duration);
        // [0.5 ... 1.0] => [0 ... 1]
        const float result = (SmoothStep(temp) - 0.5f) * 2.0f;
        return result;
    }

    void DoctorGeometry(
        VectorFloat4& in_out_geometry_pos,
        VectorFloat4& in_out_geometry_uv,
        const float in_ratio,
        const UISlideDirection in_direction
        )
    {
        const float pos_width = in_out_geometry_pos[2] - in_out_geometry_pos[0];
        const float pos_height = in_out_geometry_pos[3] - in_out_geometry_pos[1];
        const float uv_width = in_out_geometry_uv[2] - in_out_geometry_uv[0];
        const float uv_height = in_out_geometry_uv[3] - in_out_geometry_uv[1];

        switch (in_direction)
        {
        default:
            break;
        case UISlideDirection::TFromLeft:
            in_out_geometry_pos[2] = in_out_geometry_pos[0] + (in_ratio * pos_width);
            in_out_geometry_uv[0] = in_out_geometry_uv[2] - (in_ratio * uv_width);
            break;
        case UISlideDirection::TFromTop:
            in_out_geometry_pos[1] = in_out_geometry_pos[3] - (in_ratio * pos_height);
            in_out_geometry_uv[3] = in_out_geometry_uv[1] + (in_ratio * uv_height);
            break;
        case UISlideDirection::TFromRight:
            in_out_geometry_pos[0] = in_out_geometry_pos[2] - (in_ratio * pos_width);
            in_out_geometry_uv[2] = in_out_geometry_uv[0] + (in_ratio * uv_width);
            break;
        case UISlideDirection::TFromBottom:
            in_out_geometry_pos[3] = in_out_geometry_pos[1] + (in_ratio * pos_height);
            in_out_geometry_uv[1] = in_out_geometry_uv[3] - (in_ratio * uv_height);
            break;
        }

        return;
    }
}

UIComponentDrift::UIComponentDrift(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const UISlideDirection in_direction,
    const float in_duration
    )
    : _component_default(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    , _direction(in_direction)
    , _duration(in_duration)
    , _reverse(false)
{
    // Nop
}

UIComponentDrift::~UIComponentDrift()
{
    // Nop
}

const bool UIComponentDrift::Set(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const UISlideDirection in_direction,
    const float in_duration
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

    if (_direction != in_direction)
    {
        _direction = in_direction;
        dirty = true;
    }

    if (_duration != in_duration)
    {
        _duration = in_duration;
        dirty = true;
    }

    return dirty;
}

void UIComponentDrift::TriggerSlideOut()
{
    if (false == _reverse)
    {
        if (_duration < _component_default.GetTimeAccumulateSeconds())
        {
            _component_default.SetTimeAccumulateSeconds(_duration);
        }
        _reverse = true;
        _component_default.SetStateFlagBit(UIStateFlag::THidden, false);
    }
}

void UIComponentDrift::TriggerSlideIn()
{
    if (true == _reverse)
    {
        if (_component_default.GetTimeAccumulateSeconds() < 0.0f)
        {
            _component_default.SetTimeAccumulateSeconds(0.0f);
        }
        _reverse = false;
        _component_default.SetStateFlagBit(UIStateFlag::THidden, false);
    }
}

const bool UIComponentDrift::SetStateFlag(const UIStateFlag in_state_flag)
{
    return _component_default.SetStateFlag(in_state_flag);
}

const bool UIComponentDrift::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentDrift::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentDrift::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentDrift::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

void UIComponentDrift::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
{
    _component_default.SetUVScrollManual(in_uv_scroll, in_manual_horizontal, in_manual_vertical);
    return;
}

void UIComponentDrift::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentDrift::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentDrift::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    return _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        );
}

const bool UIComponentDrift::UpdateSize(
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
#if 1
    bool dirty = false;

    const float time_deta = _reverse ? -in_time_delta : in_time_delta;

    if (true == _component_default.Update(time_deta))
    {
        dirty = true;
    }

    VectorInt2 layout_size;
    VectorInt2 desired_size;
    GetDesiredSize(
        layout_size,
        desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    UIComponentDefault::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        _component_default.GetUVScroll(),
        _component_default.GetUVScrollManualX(),
        _component_default.GetUVScrollManualY(),
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta,
        layout_size,
        desired_size,
        _component_default.GetInUseLayout()
        );

    const float ratio = CalculateRatio(_duration, _component_default.GetTimeAccumulateSeconds());
    if (ratio <= 0.0f)
    {
        if (true == _component_default.SetStateFlagBit(UIStateFlag::THidden, true))
        {
            dirty = true;
        }
        return dirty;
    }
    else
    {
        if (true == _component_default.SetStateFlagBit(UIStateFlag::THidden, false))
        {
            dirty = true;
        }
    }

    if (ratio < 1.0f)
    {
        DoctorGeometry(
            geometry_pos,
            geometry_uv,
            ratio,
            _direction
            );
    }

    // Update geometry
    if (true == in_out_geometry.Set(
        geometry_pos,
        geometry_uv
        ))
    {
        dirty = true;
    }

    out_screen_space.Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    // Recurse
    in_out_node.UpdateSize(
        in_draw_system,
        texture_size, // by default, we use the generated size and scroll if required. a stack control may modify this
        VectorInt2(),
        texture_size,
        in_ui_scale,
        in_time_delta,
        dirty,
        out_screen_space
        );

    return dirty;
#else
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
#endif
}

void UIComponentDrift::GetDesiredSize(
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

const bool UIComponentDrift::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _component_default.PreDraw(
        in_draw_param,
        in_node
        );
}

const VectorFloat4 UIComponentDrift::GetTintColour() const
{
    return _component_default.GetTintColour();
}
