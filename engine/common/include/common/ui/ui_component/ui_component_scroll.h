#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/i_ui_input.h"

enum class UIOrientation;

class UIComponentScroll : public IUIComponent, public IUIInput
{
    typedef IUIComponent TSuper;
public:
    UIComponentScroll(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const UIOrientation in_orientation
        );
    virtual ~UIComponentScroll();

    const bool SetModelOther(
        const UIOrientation in_orientation
        );
    const bool Set(
        const std::function<void(const VectorFloat2&)>& in_value_change,
        const std::function<void(const VectorFloat2&)>& in_range_change,
        const VectorFloat2& in_value,
        const VectorFloat2& in_range_low_high
        );
    const float GetValueRatio() const;

    const std::function<void(const VectorFloat2&)>& GetOnValueChange() const { return _value_change; }
    const std::function<void(const VectorFloat2&)>& GetOnRangeChange() const { return _range_change; }

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual const bool UpdateSize(
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
        ) override;

    virtual void OnInputTouch(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos,
        std::string& out_tooltip
        ) override;
    virtual void OnInputClick(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos
        ) override;

private:
    UIOrientation _orientation;

    //std::shared_ptr<UIHierarchyNodeChildData> _child_data_knot;
    IUIComponent* _knot;

    std::function<void(const VectorFloat2&)> _value_change;
    std::function<void(const VectorFloat2&)> _range_change;
    VectorFloat2 _value;
    VectorFloat2 _range_low_high;

};
