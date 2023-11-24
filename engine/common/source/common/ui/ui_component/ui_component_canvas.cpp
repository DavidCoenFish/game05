#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIComponentCanvas::UIComponentCanvas(
    const UILayout& in_layout,
    void* in_source_token
    )
    : IUIComponent(
        in_layout,
        in_source_token
        )
{
    // Nop
}

UIComponentCanvas::~UIComponentCanvas()
{
    // Nop
}

