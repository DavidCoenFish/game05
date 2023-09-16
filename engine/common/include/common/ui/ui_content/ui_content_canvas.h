#pragma once

#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_content/ui_content_default.h"

class UIContentCanvas : public IUIContent
{
public:
    UIContentCanvas(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout
        );
    virtual ~UIContentCanvas();

    const bool SetBase(
        const bool in_clear_background,
        const VectorFloat4& in_clear_colour,
        const UILayout& in_layout
        );

private:
    virtual void SetSourceToken(void* in_source_ui_data_token) override;
    virtual void* GetSourceToken() const override;

    virtual const bool SetLayout(const UILayout& in_layout) override;

    virtual const bool UpdateHierarchy(
        //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void UpdateSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;

    virtual const VectorInt2 GetDesiredSize(
        const VectorInt2& in_parent_size,
        const float in_ui_scale
        ) override;

    virtual void Draw(
        const UIManagerDrawParam& in_param,
        Shader* const in_shader,
        UIGeometry* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
        ) override;

private:
    UIContentDefault _content_default;

};
