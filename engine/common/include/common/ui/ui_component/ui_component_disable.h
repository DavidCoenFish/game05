#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/math/vector_float4.h"

class ShaderConstantBuffer;
enum class UIEffectEnum;

class UIComponentDisable : public IUIComponent
{
public:
    /// weak contract to construct via factory UIManager::TContentFactory
    UIComponentDisable(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const bool in_disable = false
        );
    /// destructed by base type IUIComponent which has virtual dtor, so virtual here may be redundant but does provide info
    virtual ~UIComponentDisable();

    /// return true if modified, else false
    const bool SetBase(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout
        );

    /// return true if modified, else false
    const bool Set(
        const bool in_disable
        );

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

    /// ensure that the data structure matches the model (UIData)
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    /// convert the layout data and parent size to the texture size, geometry size and uv
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

    /// certain layout data allows shrink
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

private:
    /// composition rather than inheratence
    UIComponentDefault _component_default;

    /// the shader constants for this effect
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

    /// should this state only be expressed by the _component_default state flag?
    //bool _disable;

};
