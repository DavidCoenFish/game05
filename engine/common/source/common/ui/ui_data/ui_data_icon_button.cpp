#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_icon_button.h"

UIDataIconButton::UIDataIconButton(
    const std::string& in_icon_key,
    const bool in_enabled,
    const std::string& in_label_key,
    const std::function<void(UIDataButton*)>& in_on_click,
    const std::function<void(UIDataButton*)>& in_on_focus
    )
    : UIDataButton(
        in_enabled,
        in_label_key,
        in_on_click,
        in_on_focus
        )
    , _icon_key(in_icon_key)
{
    // Nop
}

UIDataIconButton::~UIDataIconButton()
{
    // Nop
}
