#pragma once

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IUIData;
class Shader;
class UIGeometry;
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

    virtual const bool GetClearBackground(
        VectorFloat4& out_clear_colour
        ) const = 0;

    virtual void SetSourceUIDataToken(void* in_source_ui_data_token) = 0;
    virtual void* GetSourceUIDataToken() const = 0;

    virtual void SetLayout(const UILayout& in_layout) = 0;

    virtual const bool UpdateHierarchy(
        std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) = 0;

    virtual void UpdateGeometry(
        const VectorInt2& in_parent_size,
        const float in_ui_scale, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) = 0;

    virtual const VectorInt2 GetDesiredSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale
        ) = 0;

    virtual void Draw(
        const UIManagerDrawParam& in_param,
        Shader* const in_shader,
        UIGeometry* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
        ) = 0;

};
