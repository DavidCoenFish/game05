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
    typedef std::function< void(
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
        LocaleSystem* const in_locale_system = nullptr
        );

    std::string _locale;
    float _ui_scale;
    float _time_delta;
    std::map<std::string, TContentFactory> _map_content_factory;
    DrawSystem* const _draw_system;
    ID3D12GraphicsCommandList* const _command_list;
    const IUIModel* const _ui_model;
    LocaleSystem* const _locale_system;
};

class UIHierarchyNode
{
public:
    struct LayoutData
    {
        static LayoutData FactoryFull();
        explicit LayoutData(
            const UICoord& in_size_x = UICoord(),
            const UICoord& in_size_y = UICoord(),
            const UICoord& in_attach_x = UICoord(),
            const UICoord& in_attach_y = UICoord(),
            const UICoord& in_pivot_x = UICoord(),
            const UICoord& in_pivot_y = UICoord()
            );

        // Data for how we calculate our size relative to parent
        UICoord _data_size[2];
        UICoord _data_attach[2];
        UICoord _data_pivot[2];
    };

    enum class TFlag
    {
        TNone = 0,
        TVisible = 1 << 0,

        TDefault = TVisible

        //TFocusable = 1 << 0,
        //TBlockInputToChildren = 1 << 1,
        //TChildrenFromContent = 1 << 2,

        //// Dynamic
        //TVisible = 1 << 16,
        //TFocused = 1 << 17,
        //TMouseOver = 1 << 18,
        //TInputDown = 1 << 19 // mouse or action key

    };

    explicit UIHierarchyNode(
        const LayoutData& in_layout_data = LayoutData(),
        const std::shared_ptr<IUIContent>& in_content = nullptr,
        const std::shared_ptr<UITexture>& in_texture = nullptr,
        const TFlag in_flag = TFlag::TDefault
        );
    ~UIHierarchyNode();

    const LayoutData& GetLayoutData() const { return _layout_data; }
    LayoutData& GetLayoutDataModify();

    const VectorInt2 GetTargetSize() const;

    // This may be difficult with scrolling uv going up the chain, but post update could have the accurate screen location 
    // untill anything up the chain changes. don't burn in clip to the data, filter clip in deal_input, ie, clicking on button that is outside the parent bounds
    const VectorFloat4 GetScreenCoords() const;

    UIRootInputState& GetOrMakeRootInputState();

    //return true if all bits of flga are set
    const bool GetFlag(const TFlag in_flag);
    void SetFlag(const TFlag in_flag, const bool in_value);

    // nullptr if _bUseBackBuffer is true
    std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    void AddChild(
        const std::shared_ptr<UIHierarchyNode>& in_node,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );
    void ClearChildren();

    typedef std::function< void(
        std::unique_ptr<IUIContent>& in_out_content,
        const UIContentFactoryParam& in_param
        )> TContentFactory;
    void UpdateLayout(
        const IUIData* const in_data,
        const std::string& in_model_key,
        const TContentFactory& in_factory,
        const bool in_draw_to_texture,
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
    std::shared_ptr<IUIContent> _content;
    std::shared_ptr<UITexture> _texture;
    std::shared_ptr<UIRootInputState> _root_input_state;

    LayoutData _layout_data;

    // Dynamic data as well as static? [visible, focusable, block input travel to children, children from content, focused, mouse over, mouse down
    TFlag _flag;

    std::string _data_provider_key;
    int _data_provider_change_id;

    VectorFloat4 _input_screen_size;

};





#if 0

#include "common/ui/ui_coord.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIContent;
class IUIModel;
class Shader;
class TextBlock;
class UIGeometry;
class UIHierarchyNode;
class UITexture;
struct UIManagerDrawData;

struct UIHierarchyNodeChildData
{
    UIHierarchyNodeChildData(
        const std::shared_ptr<UIHierarchyNode>& in_node,
        const std::shared_ptr<UIGeometry>& in_geometry,
        const VectorInt2& in_render_target_size
        );

    std::shared_ptr<UIHierarchyNode> _node;
    // Need to track if state changed, so not using GeometryGeneric
    //std::unique_ptr<GeometryGeneric> _geometry;
    std::shared_ptr<UIGeometry> _geometry;
    // This is over here so we can calculate with the geometry size
    VectorInt2 _render_target_size;
};

class UIHierarchyNode
{
public:
    struct LayoutData
    {
        static LayoutData FactoryFull();
        LayoutData(
            const UICoord& in_size_x,
            const UICoord& in_size_y,
            const UICoord& in_attach_x,
            const UICoord& in_attach_y,
            const UICoord& in_pivot_x,
            const UICoord& in_pivot_y
            );

        // Data for how we calculate our size relative to parent
        UICoord _data_size[2];
        UICoord _data_attach[2];
        UICoord _data_pivot[2];
    };

    enum class TFlag
    {
        TNone = 0,
        TFocusable = 1 << 0,
        TBlockInputToChildren = 1 << 1,
        TChildrenFromContent = 1 << 2,

        // Dynamic
        TVisible = 1 << 16,
        TFocused = 1 << 17,
        TMouseOver = 1 << 18,
        TInputDown = 1 << 19 // mouse or action key

    };

    static std::shared_ptr<IUIContent> MakeContentCanvas();
    static std::shared_ptr<IUIContent> MakeContentStack(
        // Horizontal or vertical
        // Margin for children?
        );
    static std::shared_ptr<IUIContent> MakeContentText(
        std::shared_ptr<TextBlock>& in_text_block
        );
    static std::shared_ptr<IUIContent> MakeContentTexture(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );

    static std::shared_ptr<UITexture> MakeTextureBackBuffer(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const bool in_allow_clear = false,
        const bool in_always_dirty = false
        );
    static std::shared_ptr<UITexture> MakeTextureRenderTarget(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f),
        const bool in_allow_clear = false,
        const bool in_always_dirty = false
        );

    // [-1.0f ... 1.0f]
    static VectorFloat4 CalculatePos(
        const VectorInt2& in_parent_size,
        const LayoutData& in_layout_data,
        const float in_ui_scale
        );

    // [0 ... in_parent_size (ie, pixels)]
    static VectorInt2 CalculateSizeInt(
        const VectorInt2& in_parent_size,
        const LayoutData& in_layout_data,
        const float in_ui_scale
        );

    UIHierarchyNode(
        const LayoutData& in_layout_data,
        const std::shared_ptr<IUIContent>& in_content,
        const std::shared_ptr<UITexture>& in_texture,
        const TFlag in_flag = TFlag::TVisible
        );
    ~UIHierarchyNode();

    const LayoutData& GetLayoutData() const { return _layout_data; }
    const VectorInt2 GetSize() const;

    //return true if all bits of flga are set
    const bool GetFlag(const TFlag in_flag);
    void SetFlag(const TFlag in_flag, const bool in_value);

    // nullptr if _bUseBackBuffer is true
    std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    void AddChild(
        const std::shared_ptr<UIHierarchyNode>& in_node,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );
    void ClearChildren();

    const bool DrawHierarchyRoot(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        Shader* const in_shader,
        const UIManagerDrawData& in_data
        );

    // return True if we needed to draw, ie, the texture may have changed
    const bool DrawHierarchy(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        Shader* const in_shader,
        const VectorInt2& in_new_size,
        const UIManagerDrawData& in_data
        );

private:
    const bool DrawInternal(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        Shader* const in_shader,
        const VectorInt2& in_parent_size,
        const UIManagerDrawData& in_data
        );

    const bool UpdateChildData(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        const IUIModel* const in_ui_model
        );

private:
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;
    std::shared_ptr<IUIContent> _content;
    std::shared_ptr<UITexture> _texture;

    LayoutData _layout_data;

    // Dynamic data as well as static? [visible, focusable, block input travel to children, children from content, focused, mouse over, mouse down
    TFlag _flag;

    // Shift into content?
    //std::function<void(UIHierarchyNode*)> _click_callback;
    //std::function<void(UIHierarchyNode*)> _focus_callback;

    std::string _data_provider_key;
    int _data_provider_change_id;

};

#endif