#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_hierarchy_node_child.h"
#include "common/ui/ui_geometry.h"

UIComponentCanvas::UIComponentCanvas()
    : IUIComponent()
{
    // Nop
}

UIComponentCanvas::~UIComponentCanvas()
{
    // Nop
}
/**/
const VectorInt2 UIComponentCanvas::GetDesiredSize(
    UIHierarchyNodeChild& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_layout_param,
    const VectorInt2& in_parent_window
    )
{
    VectorInt2 max_child_size;
    // want something indicitive of size for text with width limit
    const VectorInt2 window_size = in_component_owner.GetLayout().GetLayoutSize(in_parent_window, in_layout_param._ui_scale);

    for (auto child: in_component_owner.GetNode().GetChildData())
    {
        max_child_size = VectorInt2::Max(
            max_child_size,
            child->GetDesiredSize(
                in_layout_param,
                window_size
                )
            );
    }

    return in_component_owner.GetLayout().ApplyMargin(
        max_child_size,
        in_layout_param._ui_scale
        );
}
/**/
