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
        //std::vector<std::shared_ptr<UIData>>*& out_array_data_or_null,
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void UpdateSize(
        DrawSystem* const in_draw_system,
        const VectorInt2& in_parent_size,
        const VectorInt2& in_parent_offset,
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        const float in_time_delta, 
        UIGeometry& in_out_geometry, 
        UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        const UIScreenSpace& in_parent_screen_space,
        UIScreenSpace& out_screen_space
        ) override;

    virtual void GetDesiredSize(
        VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
        VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
        const VectorInt2& in_parent_window,
        const float in_ui_scale,
        UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        ) override;


private:
    UIContentDefault _content_default;

};
