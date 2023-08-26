#pragma once

#include "common/ui/ui_coord.h"
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

struct UIHierarchyNodeUpdateLayoutParam
{
    typedef std::function< const bool(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )> TContentFactory;

    explicit UIHierarchyNodeUpdateLayoutParam(
        const std::string& in_locale = std::string(),
        const float in_ui_scale = 1.0f,
        const float in_time_delta = 0.0f,
        const std::map<std::string, TContentFactory>& in_map_content_factory = std::map<std::string, TContentFactory>(),
        DrawSystem* const in_draw_system = nullptr,
        ID3D12GraphicsCommandList* const in_command_list = nullptr,
        const IUIModel* const in_ui_model = nullptr,
        LocaleSystem* const in_locale_system = nullptr,
        TextManager* const in_text_manager = nullptr
        );

    std::string _locale;
    float _ui_scale;
    float _time_delta;
    std::map<std::string, TContentFactory> _map_content_factory;
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    const IUIModel* const _ui_model;
    LocaleSystem* const _locale_system;
    TextManager* const _text_manager;

};


struct UIHierarchyNodeLayoutData
{
    static UIHierarchyNodeLayoutData FactoryFull();
    explicit UIHierarchyNodeLayoutData(
        const UICoord& in_size_x = UICoord(UICoord::ParentSource::X),
        const UICoord& in_size_y = UICoord(UICoord::ParentSource::Y),
        const UICoord& in_attach_x = UICoord(UICoord::ParentSource::X),
        const UICoord& in_attach_y = UICoord(UICoord::ParentSource::Y),
        const UICoord& in_pivot_x = UICoord(UICoord::ParentSource::X),
        const UICoord& in_pivot_y = UICoord(UICoord::ParentSource::Y)
        );

    // Data for how we calculate our size relative to parent
    UICoord _data_size[2];
    UICoord _data_attach[2];
    UICoord _data_pivot[2];

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

    typedef std::function< const bool(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )> TContentFactory;
    void UpdateLayout(
        const IUIData* const in_data,
        const std::string& in_model_key,
        const bool in_draw_to_texture,
        const bool in_always_dirty,
        const VectorInt2& in_target_size,
        const UIHierarchyNodeUpdateLayoutParam& in_param
        );

    // return True if we needed to draw, ie, the texture may have changed
    const bool Draw(
        const UIManagerDrawParam& in_draw_param,
        Shader* const in_shader
        );

private:
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;
    std::unique_ptr<IUIContent> _content;
    std::unique_ptr<UITexture> _texture;
    std::unique_ptr<UIRootInputState> _root_input_state;

    // would not this, and the parent size, determin our texture size? layout needs to be full screen for backbuffer?
    UIHierarchyNodeLayoutData _layout_data;

    // Dynamic data as well as static? [visible, focusable, block input travel to children, children from content, focused, mouse over, mouse down
    //TFlag _flag;

    // Push down to UITexture:_texture?
    //bool _draw_dirty;

    //std::string _data_provider_key;
    //int _data_provider_change_id;

    VectorFloat4 _input_screen_size;

};
