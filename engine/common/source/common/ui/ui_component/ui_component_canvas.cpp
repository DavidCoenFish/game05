#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIComponentCanvas::UIComponentCanvas(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
{
    // Nop
}

UIComponentCanvas::~UIComponentCanvas()
{
    // Nop
}
