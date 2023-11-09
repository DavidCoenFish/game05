#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"

class VectorInt4;
enum class UISlideDirection;

/// slide in content, and allow reverse trigger
/// rename to UIComponentDrift as slide was too close to slider
class UIComponentDrift : public IUIComponent
{
public:
    UIComponentDrift(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const UISlideDirection in_direction,
        const float in_duration
        );
    virtual ~UIComponentDrift();

    const bool Set(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const UISlideDirection in_direction,
        const float in_duration
        );

    //if slide in is playing or finished, start sliding out from current pos
    void TriggerSlideOut();
    //if slide out is playing or completed a slide out, start slidding in
    void TriggerSlideIn();

private:
    virtual const bool SetStateFlag(const UIStateFlag in_state_flag) override;
    virtual const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable) override;
    virtual const UIStateFlag GetStateFlag() const override;

    virtual const UILayout& GetLayout() const override; 
    virtual void SetLayoutOverride(const UILayout& in_override) override; 
    virtual void SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool manual_horizontal, const bool manual_vertical) override;

    /// Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    /// Make sorting children easier
    virtual void* GetSourceToken() const override;

    /// make the hirearchy match the model (UIData)
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    /// convert the layout data and parent size to the texture size, geometry size and uv
    /// certain component types may do slightly different operations, but the default is to call GetDesiredSize
    virtual const bool UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text,
        const UIScreenSpace& in_parent_screen_space,
        UIScreenSpace& out_screen_space
        ) override;

    /// certain layout data allows shrink, and certain componets may have different logic, such as stack component
    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    /// deal with the component being drawn to the node texture
    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

    virtual const VectorFloat4 GetTintColour() const override;

private:
    /// Default properties of a component
    UIComponentDefault _component_default;

    bool _reverse;
    UISlideDirection _direction;
    float _duration;

};
