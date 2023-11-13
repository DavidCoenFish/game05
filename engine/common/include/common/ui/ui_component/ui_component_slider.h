#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/i_ui_input.h"

enum class UIOrientation;

// child is the slider knot?
class UIComponentSlider : public IUIComponent, public IUIInput
{
    typedef IUIComponent TSuper;
public:
    UIComponentSlider(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const UIOrientation in_orientation
        );
    virtual ~UIComponentSlider();

    const bool SetModelOther(
        const UIOrientation in_orientation
        );
    const bool Set(
        const std::function<void(const float)>& in_value_change,
        const std::function<const std::string()>& in_get_tooltip,
        const float in_value,
        const VectorFloat2& in_range_low_high
        );

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
    virtual void OnHover(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_touch_pos,
        std::string& out_tooltip
        ) override;

private:
    UIOrientation _orientation;

    // Is it simpler to just have the last child node be the knot? but then we have to control how it's layout is updated
    //std::shared_ptr<UIHierarchyNodeChildData> _child_data_knot;

    /// Reference to the know that was somewhere under us in the node hierarchy
    IUIComponent* _knot;

    std::function<void(const float)> _value_change;
    std::function<const std::string()> _get_tooltip;

    float _value;
    VectorFloat2 _range_low_high;

};
