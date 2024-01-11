#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_button.h"

#include "common/ui/ui_component/ui_component_button.h"

UIDataButton::UIDataButton(
#ifdef _DEBUG
    const std::string& in_debug_name,
#endif
    const UILayout& in_layout,
    const UIBaseColour& in_base_colour,
    const UITintColour& in_tint_colour,
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    const TOnClick& in_on_click,
    const TGetTooltip& in_get_tooltip,
    const bool in_repeat
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
    , _on_click(in_on_click)
    , _get_tooltip(in_get_tooltip)
    , _repeat(in_repeat)
{
    // Nop
}

UIDataButton::~UIDataButton()
{
    // Nop
}

const bool UIDataButton::ApplyComponent(
    std::unique_ptr<IUIComponent>& in_out_component,
    const UIHierarchyNodeUpdateParam& //in_param
    )
{
    // if in_out_component is not a UIComponentButton, remake it as one
    UIComponentButton* component = dynamic_cast<UIComponentButton*>(in_out_component.get());

    bool dirty = false;
    if (nullptr == component)
    {
        auto new_component = std::make_unique<UIComponentButton>(
            _on_click,
            _get_tooltip,
            _repeat
            );
        in_out_component = std::move(new_component);
        dirty = true;
    }
    else
    {
        // we don't set the token source again, as we expect upstream to destroy the component if source token doesn't match
        if (true == component->Set(
            _on_click,
            _get_tooltip,
            _repeat
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

