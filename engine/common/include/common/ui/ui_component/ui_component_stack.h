#pragma once

#include "common/ui/ui_component/i_ui_component.h"

class VectorInt4;
enum class UIOrientation;

class UIComponentStack : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentStack(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const UIOrientation in_orientation,
        const UICoord& in_gap
        );
    virtual ~UIComponentStack();

    const bool SetModelOther(
        const UIOrientation in_orientation,
        const UICoord& in_gap
        );

private:
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

    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    /// Stack is a little special in that we want the children to expand as needed along one axis
    /// skip over null child components
    void GetStackDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        std::vector<VectorInt4>& out_child_window_offset
        );

private:
    UIOrientation _orientation;
    UICoord _gap;

};
