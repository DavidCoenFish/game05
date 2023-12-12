#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIComponentCanvas::UIComponentCanvas(
    const UILayout& in_layout,
    const UITintColour& in_tint_colour,
    void* in_source_token
    )
    : IUIComponent(
        in_layout,
        in_tint_colour,
        in_source_token
        )
{
    // Nop
}

UIComponentCanvas::~UIComponentCanvas()
{
    // Nop
}

const bool UIComponentCanvas::Set(
    const UILayout& in_layout,
    const UITintColour& in_tint_colour
    )
{
    SetLayout(in_layout);
    SetTintColour(in_tint_colour);
    return false;
}

const VectorInt2 UIComponentCanvas::GetDesiredSize(
    UIHierarchyNodeChildData& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_layout_param,
    const VectorInt2& in_parent_window
    )
{
    VectorInt2 max_child_size;
    // want something indicitive of size for text with width limit
    const VectorInt2 window_size = GetLayout().GetLayoutSize(in_parent_window, in_layout_param._ui_scale);

    for (auto child: in_component_owner._node->GetChildData())
    {
        if (nullptr == child->_component)
        {
            continue;
        }
        max_child_size = VectorInt2::Max(
            max_child_size,
            child->_component->GetDesiredSize(
                *child,
                in_layout_param,
                window_size
                )
            );
    }

    return GetLayout().ApplyMargin(
        max_child_size,
        in_layout_param._ui_scale
        );
}

