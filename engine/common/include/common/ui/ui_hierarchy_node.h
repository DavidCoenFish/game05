#pragma once

#include "common/ui/ui_coord.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIData;
class IUIContent;
class IUIModel;
class LocaleSystem;
class Shader;
class TextManager;
class UIGeometry;
class UIHierarchyNode;
class UIRootInputState;
class UITexture;
class UILayout;
struct UIContentFactoryParam;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;

/*
UIHierarchyNode // N0 
    _texture // T0 texture or backbuffer A0 draws to
    //_input_state // I0 top level input state, which node is focused, node click started on, hover node
    _child_node_array // A0
        _geometry // G1 geometry to draw the texture T1 onto T0
        _content // C1 controls size of T1 and G1. model returns an array of ui data for root
        //_layoutData // L0 layout data here or in content
        UIHierarchyNode // N1 child node
            _texture // T1 texture or backbuffer A1 draws to
            _child_node_array // A1

    _content->UpdateGeometry(in_parent_size, _geometry, node)
    VectorInt2 UIHierarchyNode::GetDesiredSize(in_parent_size) // text may bt told to limit width to in_parent_size
*/

struct UIHierarchyNodeChildData
{
    UIHierarchyNodeChildData(
        std::unique_ptr<UIGeometry>& in_geometry,
        std::unique_ptr<IUIContent>& in_content,
        std::unique_ptr<UIHierarchyNode>& in_node,
        const VectorFloat4& in_input_screen_size = VectorFloat4()
        );
    ~UIHierarchyNodeChildData();

    // Need to track if state changed, so not using GeometryGeneric
    std::unique_ptr<UIGeometry> _geometry;
    // _content is out here rather than in _node to avoid top level node needing a root or do nothing content for layout
    std::unique_ptr<IUIContent> _content; 
    std::unique_ptr<UIHierarchyNode> _node;
    VectorFloat4 _input_screen_size;
    //std::unique_ptr<UILayout>(); //layout data here or in content?

};

struct UIHierarchyNodeUpdateHierarchyParam
{
    typedef std::function< const bool(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )> TContentFactory;

    explicit UIHierarchyNodeUpdateHierarchyParam(
        const std::map<std::string, TContentFactory>& in_map_content_factory = std::map<std::string, TContentFactory>(),
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        const IUIModel* const in_ui_model = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr
        );
    const std::map<std::string, TContentFactory>& _map_content_factory;
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    const IUIModel* const _ui_model;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;

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

    // nullptr if _bUseBackBuffer is true
    std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    void AddChild(
        std::unique_ptr<IUIContent>& in_content
        );
    const bool ClearChildren();

    // Expose child data array to allow ui_content to specialise how hieararchy builds
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& GetChildData() { return _child_data_array; }

    // create/ destroy nodes to match model, make content match type from factory, update content?
    const bool UpdateHierarchy(
        const UIHierarchyNodeUpdateHierarchyParam& in_param,
        const std::vector<std::shared_ptr<IUIData>>* const in_array_data_or_null,
        const bool in_draw_to_texture,
        const bool in_always_dirty,
        const bool in_allow_clear = false,
        const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    
    const VectorInt2 GetTextureSize(
        DrawSystem* const in_draw_system
        ) const;

    void UpdateSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale,
        const float in_time_delta,
        const bool in_mark_dirty
        );

    //const VectorInt2 GetDesiredSize(
    //    const VectorInt2& in_parent_size,
    //    const float in_ui_scale
    //    );

    // return True if we needed to draw, ie, we have modified _texture
    const bool Draw(
        const UIManagerDrawParam& in_draw_param,
        Shader* const in_shader
        );

private:
    std::unique_ptr<UITexture> _texture;
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;

    // Allow top level node to hold some state for input like the focused node, or click active node, hover node?
    // Lazy create
    std::unique_ptr<UIRootInputState> _root_input_state;

};
