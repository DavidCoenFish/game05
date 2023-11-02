#pragma once

#include "common/ui/ui_coord.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class UIData;
class UIManager;
class IUIComponent;
class IUIModel;
class LocaleSystem;
class Shader;
class ShaderConstantBuffer;
class TextManager;
class UIGeometry;
class UIHierarchyNode;
class UIRootInputState;
class UITexture;
class UILayout;
class UIScreenSpace;

struct UIComponentFactoryParam;
struct UIDataTextRunStyle;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;

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

the component C1 may have special rules (ie, a virtual method) to control how child array A1 is drawn to T1
put tint colour into geometry, how about the shader constants?

*/

struct UIHierarchyNodeChildData
{
    UIHierarchyNodeChildData(
        std::unique_ptr<UIGeometry>& in_geometry,
        std::unique_ptr<IUIComponent>& in_component,
        std::unique_ptr<UIHierarchyNode>& in_node,
        std::unique_ptr<UIScreenSpace>& in_screen_space
        );
    ~UIHierarchyNodeChildData();

    /// Need to track if state changed, so not using GeometryGeneric
    std::unique_ptr<UIGeometry> _geometry;
    /// _component is out here rather than in _node to avoid top level node needing a root or do nothing content for layout
    std::unique_ptr<IUIComponent> _component; 
    /// recusion point
    std::unique_ptr<UIHierarchyNode> _node;
    /// size relative to screen for input
    std::unique_ptr<UIScreenSpace> _screen_space;
    /// shader constants to allow for tint
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

};

struct UIHierarchyNodeUpdateHierarchyParam
{
    typedef std::function< const bool(
        std::unique_ptr<IUIComponent>& in_out_content,
        const UIComponentFactoryParam& in_param
        )> TContentFactory;

    explicit UIHierarchyNodeUpdateHierarchyParam(
        const std::map<std::string, TContentFactory>& in_map_content_factory = std::map<std::string, TContentFactory>(),
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        const IUIModel* const in_ui_model = nullptr,
        UIManager* const in_ui_manager = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr,
        const UIDataTextRunStyle* const in_default_text_style = nullptr
        );
    const std::map<std::string, TContentFactory>& _map_content_factory;
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    const IUIModel* const _ui_model;
    UIManager* const _ui_manager;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;
    const UIDataTextRunStyle* const _default_text_style;

};

struct UIHierarchyNodeUpdateDesiredSize
{
    explicit UIHierarchyNodeUpdateDesiredSize(
        const float in_ui_scale = 1.0f,
        const float in_time_delta = 0.0f
        );
    const float _ui_scale;
    const float _time_delta;

};

struct UIHierarchyNodeUpdateSize
{
    explicit UIHierarchyNodeUpdateSize();
};

class UIHierarchyNode
{
public:
    UIHierarchyNode();
    ~UIHierarchyNode();

    UIRootInputState& GetOrMakeRootInputState();

    /// nullptr if _bUseBackBuffer is true
    //std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    void AddChild(
        std::unique_ptr<IUIComponent>& in_content
        );
    const bool ClearChildren();

    /// Expose child data array to allow ui_component to specialise how hieararchy builds
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& GetChildData() { return _child_data_array; }

    /// create/ destroy nodes to match model, make content match type from factory, update content?
    const bool UpdateHierarchy(
        const UIHierarchyNodeUpdateHierarchyParam& in_param,
        const std::vector<std::shared_ptr<UIData>>* const in_array_data_or_null,
        const bool in_draw_to_texture,
        const bool in_always_dirty,
        const bool in_allow_clear = false,
        const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    
    void MarkTextureDirty();

    const VectorInt2 GetTextureSize(
        DrawSystem* const in_draw_system
        ) const;

    void UpdateTextureSize(
        const VectorInt2& in_parent_size,
        const bool in_mark_dirty
        );

    void UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta,
        const bool in_mark_dirty,
        const UIScreenSpace& in_parent_screen_space
        );

    void DealInput(
        UIRootInputState& in_input_state,
        const int in_state_flag = 0
        );


    /// split PreDraw and Draw to allow UIComponentEffect to prep the childrent but implement it's own texture draw
    /// return True if we needed to draw, ie, we have modified children. Ask children to update there textures
    const bool PreDraw(
        const UIManagerDrawParam& in_draw_param
        );
    /// return True if we needed to draw, ie, we have modified _texture. Draw children textures to our own texture
    const bool Draw(
        const UIManagerDrawParam& in_draw_param,
        const bool in_dirty
        );

    UITexture& GetUITexture() const { return *_texture; }

private:
    /// recursion data structure, holds a UIHierarchyNode, component, geometry...
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;

    /// Hold the render target or wrap the backbuffer as a texture
    std::unique_ptr<UITexture> _texture;

    /// Allow top level node to hold some state for input like the focused node, or click active node, hover node?
    std::unique_ptr<UIRootInputState> _root_input_state;

};
