#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_button.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIContentButton::UIContentButton(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
    : _content_default(
        in_clear_background,
        in_clear_colour,
        in_layout
        )
{
    // Nop
}

UIContentButton::~UIContentButton()
{
    // Nop
}

const bool UIContentButton::SetBase(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
{
    return _content_default.SetBase(
        in_clear_background,
        in_clear_colour,
        in_layout
        );
}

void UIContentButton::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIContentButton::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIContentButton::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIContentButton::UpdateHierarchy(
    //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
    IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    return _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        );
}

void UIContentButton::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _content_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node
        );
}

void UIContentButton::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    return _content_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );
}
