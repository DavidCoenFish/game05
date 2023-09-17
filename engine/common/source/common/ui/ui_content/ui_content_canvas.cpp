#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIContentCanvas::UIContentCanvas(
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

UIContentCanvas::~UIContentCanvas()
{
    // Nop
}

const bool UIContentCanvas::SetBase(
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

void UIContentCanvas::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIContentCanvas::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIContentCanvas::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIContentCanvas::UpdateHierarchy(
    //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
    const IUIData* const in_data,
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

void UIContentCanvas::UpdateSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _content_default.UpdateSize(
        in_parent_size,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node
        );
}

const VectorInt2 UIContentCanvas::GetDesiredSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale
    )
{
    return _content_default.GetDesiredSize(
        in_parent_size,
        in_ui_scale
        );
}
