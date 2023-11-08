#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/i_ui_input.h"

class UIComponentButton : public IUIComponent, public IUIInput
{
public:
    UIComponentButton(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const std::function<void(const VectorFloat2&)>& in_on_click = nullptr,
        const bool in_allow_repeat = false
        );
    virtual ~UIComponentButton();

    /// Called from factory
    const bool SetBase(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );

    const bool Set(
        const std::function<void(const VectorFloat2&)>& in_on_click,
        const bool in_allow_repeat
        );

private:
    virtual const bool SetStateFlag(const UIStateFlag in_state_flag) override;
    virtual const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable) override;
    virtual const UIStateFlag GetStateFlag() const override;

    virtual const UILayout& GetLayout() const override; 
    virtual void SetLayoutOverride(const UILayout& in_override) override; 
    virtual void SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool manual_horizontal, const bool manual_vertical) override;

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

    virtual void OnInputClick(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos
        ) override;

    virtual void OnInputRepeat(
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos,
        const float in_duration,
        const float in_last_timestep
        );

    virtual const VectorFloat4 GetTintColour() const override;

private:
    UIComponentDefault _component_default;
    std::function<void(const VectorFloat2&)> _on_click;
    bool _allow_repeat;

};
