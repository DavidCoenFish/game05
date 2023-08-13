#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_button.h"

UIDataButton::UIDataButton(
    const bool in_enabled,
    const std::string& in_label_key,
    const std::function<void(UIDataButton*)>& in_on_click,
    const std::function<void(UIDataButton*)>& in_on_focus
    )
    : _enabled(in_enabled)
    , _label_key(in_label_key)
    , _on_click(in_on_click)
    , _on_focus(in_on_focus)
{
    // Nop
}

UIDataButton::~UIDataButton()
{
    // Nop
}

void UIDataButton::OnClick()
{
    if (_on_click)
    {
        _on_click(this);
    }
}

void UIDataButton::OnFocus()
{
    if (_on_focus)
    {
        _on_focus(this);
    }
}
