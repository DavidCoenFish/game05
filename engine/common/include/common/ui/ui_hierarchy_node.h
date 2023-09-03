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

struct UIHierarchyNodeChildData
{
    UIHierarchyNodeChildData(
        const std::shared_ptr<UIHierarchyNode>& in_node,
        const std::shared_ptr<UIGeometry>& in_geometry//,
        //const VectorInt2& in_render_target_size
        );

    std::shared_ptr<UIHierarchyNode> _node;
    // Need to track if state changed, so not using GeometryGeneric
    std::shared_ptr<UIGeometry> _geometry;
    // This is over here so we can calculate with the geometry size, or better to get from _node and not duplicate state
    //VectorInt2 _target_size;
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

    //enum class TFlag
    //{
    //    TNone = 0,
    //    TVisible = 1 << 0,

    //    TDefault = TVisible

    //    //TFocusable = 1 << 0,
    //    //TBlockInputToChildren = 1 << 1,
    //    //TChildrenFromContent = 1 << 2,

    //    //// Dynamic
    //    //TVisible = 1 << 16,
    //    //TFocused = 1 << 17,
    //    //TMouseOver = 1 << 18,
    //    //TInputDown = 1 << 19 // mouse or action key

    //};

    UIHierarchyNode();

    //explicit UIHierarchyNode(
    //    const LayoutData& in_layout_data = LayoutData(),
    //    std::unique_ptr<IUIContent>& in_content = std::unique_ptr<IUIContent>(),
    //    std::unique_ptr<UITexture>& in_texture = std::unique_ptr<UITexture>()//,
    //    //const TFlag in_flag = TFlag::TDefault
    //    );
    ~UIHierarchyNode();

    const UIHierarchyNodeLayoutData& GetLayoutData() const { return _layout_data; }
    UIHierarchyNodeLayoutData& GetLayoutDataModify();

    const VectorInt2 GetTargetSize() const;

    // This may be difficult with scrolling uv going up the chain, but post update could have the accurate screen location 
    // untill anything up the chain changes. don't burn in clip to the data, filter clip in deal_input, ie, clicking on button that is outside the parent bounds
    const VectorFloat4 GetScreenCoords() const;

    UIRootInputState& GetOrMakeRootInputState();

    //return true if all bits of flga are set
    //const bool GetFlag(const TFlag in_flag);
    //void SetFlag(const TFlag in_flag, const bool in_value);

    // nullptr if _bUseBackBuffer is true
    std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    void AddChild(
        const std::shared_ptr<UIHierarchyNode>& in_node,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );
    void ClearChildren();

    // create/ destroy nodes to match model, make content match type from factory, update content?
    const bool UpdateHierarchy(
        const UIHierarchyNodeUpdateHierarchyParam& in_param,
        const IUIData& in_data,
        const bool in_draw_to_texture,
        const bool in_always_dirty
        );

    // work out what size everything wants to be
    void UpdateDesiredSize(
        const VectorInt2& in_parent_desired_size,
        const UIHierarchyNodeUpdateDesiredSize& in_param
        );

    // Set texture size, geometry size, _input_screen_size. when desired size is bigger than parent, auto or manual scroll
    // Shrink parent if requested
    // Clamping click area on parent would require passing vector of masked areas? or vector of unmasked? too much pain, don't clamp?
    void UpdateSize(
        const VectorInt2& in_parent_size,
        const VectorFloat4& in_input_screen_parent,
        const UIHierarchyNodeUpdateSize& in_param
        );

    // Update hover, focus, button click, drag?
    void DealInput(UIRootInputState& input_state);

    // return True if we needed to draw, ie, we have modified _texture
    const bool Draw(
        const UIManagerDrawParam& in_draw_param,
        Shader* const in_shader
        );

private:
    std::unique_ptr<IUIContent> _content;
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;
    std::unique_ptr<UITexture> _texture;
    std::unique_ptr<UILayout> _layout;

    // Allow top level node to hold some state for input like the focused node, or click active node, hover node?
    std::unique_ptr<UIRootInputState> _root_input_state;

    // Move to inside content? or add flags to deal with shiring x/y for text or stack

    // Dynamic data as well as static? [visible, focusable, block input travel to children, children from content, focused, mouse over, mouse down
    //TFlag _flag;

    // Push down to UITexture:_texture?
    //bool _draw_dirty;

    //std::string _data_provider_key;
    //int _data_provider_change_id;

    VectorFloat4 _input_screen_size;

};
