#pragma once

#include "common/ui/ui_layout.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIData;
class Shader;
class UIGeometry;
class UITexture;
class UIHierarchyNode;
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

    const bool SetBase(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout
        );

    void SetSourceToken(void* in_source_ui_data_token);
    void* GetSourceToken() const;

    const bool SetLayout(const UILayout& in_layout);

    const bool UpdateHierarchy(
        //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        );

    void UpdateSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        );

    const VectorInt2 GetDesiredSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale
        );

    void Draw(
        const UIManagerDrawParam& in_param,
        Shader* const in_shader,
        UIGeometry* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
        );

private:
    bool _clear_background;
    VectorFloat4 _clear_colour;
    UILayout _layout;
    void* _source_token;

};
