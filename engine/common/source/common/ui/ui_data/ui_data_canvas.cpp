#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_canvas.h"

#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_screen_space.h"

const bool UIDataCanvas::ApplyComponent(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam&, //in_param,
    const int //in_index
    )
{
    bool dirty = false;
    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentCanvas* component = dynamic_cast<UIComponentCanvas*>(in_out_component.get());
    if (nullptr == component)
    {
        auto new_component = std::make_unique<UIComponentCanvas>();
        //component = new_component.get();
        in_out_component = std::move(new_component);
        dirty = true;
    }

    return dirty;
}
