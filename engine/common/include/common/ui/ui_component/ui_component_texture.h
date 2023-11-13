#pragma once

#include "common/ui/ui_component/i_ui_component.h"


class DrawSystemFrame;
class HeapWrapperItem;
class Shader;
class ShaderConstantBuffer;
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

if _component C1 is our UIComponentTexture, it has an custom set texture which is drawn to _texture T1 using our local geometry
_geometry G1 is then used by the UIHierarchyNode::Draw of N0 to draw T1 to T0
*/

class UIComponentTexture : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentTexture(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle = nullptr,
        const std::shared_ptr<IResource>& in_shader_resource = nullptr
        );
    virtual ~UIComponentTexture();

    void SetTexture(
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle,
        const std::shared_ptr<IResource>& in_shader_resource
        );

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

    virtual const VectorFloat4 GetTintColour() const override;

private:
    bool _dirty;
    std::shared_ptr<HeapWrapperItem> _shader_resource_view_handle;
    std::shared_ptr<IResource> _shader_resource;
    std::unique_ptr<UIGeometry> _geometry;
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

};
