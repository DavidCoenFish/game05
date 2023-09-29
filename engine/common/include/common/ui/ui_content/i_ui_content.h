#pragma once

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIData;
class Shader;
class UIGeometry;
class UIHierarchyNode;
class UILayout;
class UITexture;
class VectorInt2;
class VectorFloat4;

struct UIManagerDrawData;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;
struct UIManagerDealInputParam;
struct UIHierarchyNodeChildData;
struct UIHierarchyNodeLayoutData;
struct UIHierarchyNodeUpdateHierarchyParam;

class IUIContent
{
public:
    IUIContent();
    virtual ~IUIContent();

    //virtual const bool GetClearBackground(
    //    VectorFloat4& out_clear_colour
    //    ) const = 0;

    // Make sorting children easier
    virtual void SetSourceToken(void* in_source_ui_data_token) = 0;
    virtual void* GetSourceToken() const = 0;

    virtual const bool SetLayout(const UILayout& in_layout) = 0;

    virtual const bool UpdateHierarchy(
        IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) = 0;

    virtual void UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) = 0;

    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) = 0;

    //virtual const bool GetNeedsPreDraw() const;
    virtual void PreDraw(
        const UIManagerDrawParam& in_param
        );

};
