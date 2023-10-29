#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/i_ui_input.h"

enum class UIOrientation;
/*
class UIComponentScroll : public IUIComponent, public IUIInput
{
public:
    UIComponentScroll(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const UICoord& in_minimum_knot_thickness,
        const UIOrientation _orientation
        );
    virtual ~UIComponentScroll();

    const bool SetBase(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const UICoord& in_minimum_knot_thickness,
        const UIOrientation _orientation
        );
    const bool Set(
        const std::function<void(const float)>& in_value_change,
        const VectorFloat2& in_value_low_high,
        const VectorFloat2& in_range_low_high
        );

private:
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    virtual void* GetSourceToken() const override;

    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void UpdateSize(
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

    virtual void OnInputMouseClick(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos
        ) override;

    virtual const bool Draw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

private:
    UIComponentDefault _content_default;
    std::function<void(const VectorFloat2&)> _on_click;
    VectorFloat2 _value_low_high;
    VectorFloat2 _range_low_high;

};
*/