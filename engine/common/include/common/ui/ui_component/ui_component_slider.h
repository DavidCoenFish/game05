#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/i_ui_input.h"

enum class UIOrientation;

// child is the slider knot?
class UIComponentSlider : public IUIComponent, public IUIInput
{
public:
    UIComponentSlider(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const UIOrientation in_orientation
        );
    virtual ~UIComponentSlider();

    const bool SetBase(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const UIOrientation in_orientation
        );
    const bool Set(
        const std::function<void(const float)>& in_value_change,
        const float in_value,
        const VectorFloat2& in_range_low_high
        );

private:
    virtual const bool SetStateFlag(const UIStateFlag in_state_flag) override;
    virtual const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable) override;
    virtual const UIStateFlag GetStateFlag() const override;

    virtual const UILayout& GetLayout() const override; 
    virtual void SetLayoutOverride(const UILayout& in_override) override; 

    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    virtual void* GetSourceToken() const override;

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

    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;
    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

    virtual void OnInputTouch(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos
        );
    virtual void OnInputClick(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos
        ) override;

private:
    UIComponentDefault _component_default;

    UIOrientation _orientation;

    // Is it simpler to just have the last child node be the knot? but then we have to control how it's layout is updated
    //std::shared_ptr<UIHierarchyNodeChildData> _child_data_knot;
    IUIComponent* _knot;

    std::function<void(const float)> _value_change;
    float _value;
    VectorFloat2 _range_low_high;

};
