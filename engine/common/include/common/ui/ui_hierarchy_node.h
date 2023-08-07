#pragma once

#include "common/ui/ui_coord.h"

class DrawySystem;
class IUIContent;
class TextBlock;
class UIGeometry;
class UITexture;
class VectorInt2;

class UIHierarchyNode
{
public:
    struct LayoutData
    {
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

    static std::unique_ptr<IUIContent> MakeContentCanvas(
        // Children source callback
        );
    static std::unique_ptr<IUIContent> MakeContentStack(
        // Horizontal or vertical
        // Margin for children
        // Children source callback
        );
    static std::unique_ptr<IUIContent> MakeContentText(
        std::shared_ptr<TextBlock>& in_text_block
        );

    static std::unique_ptr<UITexture> MakeTextureBackBuffer(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );
    static std::unique_ptr<UITexture> MakeTextureRenderTarget(
        DrawySystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorInt2& in_size
        );

    static VectorFloat4 CalculatePos(
        const VectorInt2& in_parent_size,
        const LayoutData& in_layout_data,
        const float in_ui_scale
        );

    static VectorInt2 CalculateSizeInt(
        const VectorInt2& in_parent_size,
        const LayoutData& in_layout_data,
        const float in_ui_scale
        );

    UIHierarchyNode(
        const LayoutData& in_layout_data,
        std::unique_ptr<IUIContent>& in_content,
        std::unique_ptr<UITexture>& in_texture,
        const TFlag in_flag = TFlag::TVisible
        );
    ~UIHierarchyNode();

    const LayoutData& GetLayoutData() const { return _layout_data; }
    const VectorInt2 GetSize() const;

    //return true if all bits of flga are set
    const bool GetFlag(const TFlag in_flag);
    void SetFlag(const TFlag in_flag, const bool in_value);

    void AddChild(
        std::unique_ptr<UIHierarchyNode>& in_node,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );
    void ClearChildren();
    // id by name? don't support remove? dynamic child source is from content?
    //void RemoveChild(UIHierarchyNode* const in_node);

    // UIManager has register UIHierarchyNode by name?

private:
    struct ChildData
    {
        std::unique_ptr<UIHierarchyNode> _node;
        // Need to track if state changed, so not using GeometryGeneric
        //std::unique_ptr<GeometryGeneric> _geometry;
        std::unique_ptr<UIGeometry> _geometry;
    };

    std::vector<std::shared_ptr<ChildData>> _child_data_array;
    std::unique_ptr<IUIContent> _content;
    std::unique_ptr<UITexture> _texture;

    LayoutData _layout_data;

    // Dynamic data as well as static? [visible, focusable, block input travel to children, children from content, focused, mouse over, mouse down
    TFlag _flag;

    // Shift into content?
    //std::function<void(UIHierarchyNode*)> _click_callback;
    //std::function<void(UIHierarchyNode*)> _focus_callback;

};
