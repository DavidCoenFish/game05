#pragma once

#include "common/ui/ui_layout.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIContent;
class UIData;
class Shader;
class UIGeometry;
class UITexture;
class UIHierarchyNode;
class UIScreenSpace;
class VectorInt2;
class VectorFloat4;

struct UIManagerDrawData;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;
struct UIManagerDealInputParam;
struct UIHierarchyNodeChildData;
struct UIHierarchyNodeLayoutData;
struct UIHierarchyNodeUpdateHierarchyParam;

/*
    don't inherit functionality, but we can share methods/ components
    default implementation
*/
class UIContentDefault
{
public:
    UIContentDefault(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout,
        void* in_source_token = nullptr
        );
    ~UIContentDefault();

    static void CalculateGeometry(
        VectorFloat4& out_geometry_pos,
        VectorFloat4& out_geometry_uv,
        VectorInt2& out_texture_size,
        VectorFloat2& in_out_scroll,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        const VectorInt2& in_layout_size, // layout size != in_parent_window, unless layout size is 100% of the parent window...
        const VectorInt2& in_desired_size,
        const UILayout& in_layout 
        );

    const bool SetBase(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout
        );

    void SetSourceToken(void* in_source_ui_data_token);
    void* GetSourceToken() const;

    const bool SetLayout(const UILayout& in_layout);
    const UILayout& GetLayout() const { return _layout; }

    VectorFloat2& GetUVScroll() { return _uv_scroll; }

    const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        );

    void UpdateSize(
        DrawSystem* const in_draw_system,
        IUIContent& in_out_ui_content,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        const UIScreenSpace& in_parent_screen_space,
        UIScreenSpace& out_screen_space
        );

    void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        );

private:
    VectorFloat4 _clear_colour;
    // uv = abs(_uv_scroll), and use range [-1...1] wrapped when advancing _uv_scroll, to allow saw tooth animation
    // Scale update speed by desired size ratio to target size?
    VectorFloat2 _uv_scroll;
    UILayout _layout;
    void* _source_token;
    bool _clear_background;

};
