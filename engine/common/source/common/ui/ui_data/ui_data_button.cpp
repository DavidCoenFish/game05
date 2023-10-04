#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_button.h"

UIDataButton::UIDataButton(
    const std::function<void(const VectorFloat2&)>& in_on_click,
    const bool in_enabled,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _enabled(in_enabled)
    , _on_click(in_on_click)
{
    // Nop
}

UIDataButton::~UIDataButton()
{
    // Nop
}

