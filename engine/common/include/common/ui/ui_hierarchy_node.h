#pragma once

#include "common/ui/ui_coord.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIContent;
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
        const IUIProviderData* const in_ui_provider_data
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
