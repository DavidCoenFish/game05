#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_stack.h"

#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_component/ui_component_stack.h"

UIDataStack::UIDataStack(
#ifdef _DEBUG
    const std::string& in_debug_name,
#endif
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const UITintColour& in_tint_colour,
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    const UIOrientation in_orientation,
    const UICoord& in_gap
    )
    : UIData(
#ifdef _DEBUG
        in_debug_name,
#endif
        in_layout,
        in_base_colour,
        in_tint_colour,
        in_array_effect_data
        )
    , _orientation(in_orientation)
    , _gap(in_gap)
{
    // Nop
}

UIDataStack::~UIDataStack()
{
    // Nop
}

const bool UIDataStack::ApplyComponent(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam& //in_param
    )
{
    // if in_out_component is not a UIComponentCanvas, remake it as one
    UIComponentStack* content = dynamic_cast<UIComponentStack*>(in_out_component.get());

    bool dirty = false;
    if (nullptr == content)
    {
        auto new_content = std::make_unique<UIComponentStack>(
            _orientation,
            _gap
            );
        content = new_content.get();
        in_out_component = std::move(new_content);
        dirty = true;
    }
    else
    {
        // we don't set the token source again, as we expect upstream to destroy the component if source token doesn't match
        if (true == content->Set(
            _orientation,
            _gap
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

