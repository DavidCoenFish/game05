#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"

class DrawSystemFrame;
class HeapWrapperItem;
class Shader;
class IResource;
class UIGeometry;
class UITexture;
struct UIHierarchyNodeChildData;

/*
UIHierarchyNode // N0 
    _texture // T0 texture or backbuffer A0 draws to
    [_input_state] // I0 optional top level input state, which node is focused, node click started on, hover node
    _child_node_array // A0
        _geometry // G1 geometry to draw the texture T1 onto T0
        _shader_constant_buffer // S1 the shader constants -> moved to component default? or needs to be with? in? geometry G1
        _component // C1 controls size of T1 and G1. model returns an array of ui data for child array A1
        UIHierarchyNode // N1 child node
            _texture // T1 texture or backbuffer A1 draws to
            _child_node_array // A1

if _component C1 is our UIComponentTexture, it has an custom set texture which is drawn to _texture T1 using out local geometry
_geometry G1 is then used by the UIHierarchyNode::Draw to draw T1 to T0
*/

class UIComponentTexture : public IUIComponent
{
public:
    UIComponentTexture(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle,
        const std::shared_ptr<IResource>& in_shader_resource
        );
    virtual ~UIComponentTexture();

    void SetTexture(
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle,
        const std::shared_ptr<IResource>& in_shader_resource
        );

private:
    virtual const bool SetStateFlag(const UIStateFlag in_state_flag) override;
    virtual const bool SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable) override;
    virtual const UIStateFlag GetStateFlag() const override;

    virtual const UILayout& GetLayout() const override; 
    virtual void SetLayoutOverride(const UILayout& in_override) override; 

    // Make sorting children easier
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

private:
    UIComponentDefault _component_default;

    bool _dirty;
    std::shared_ptr<HeapWrapperItem> _shader_resource_view_handle;
    std::shared_ptr<IResource> _shader_resource;
    std::unique_ptr<UIGeometry> _geometry;
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

};
