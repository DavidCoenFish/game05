#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_button.h"

UIDataButton::UIDataButton(
    const TOnValueChange& in_on_click,
    const TGetTooltip& in_get_tooltip,
    const bool in_repeat,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
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

