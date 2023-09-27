#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_button.h"

UIDataButton::UIDataButton(
    const bool in_enabled,
    const std::shared_ptr<IUIData>& in_content,
    const std::function<void(const VectorFloat2&)>& in_on_click,
    const std::string& in_template_name
    )
    : _enabled(in_enabled)
    , _content(in_content)
    , _on_click(in_on_click)
    , _template_name(in_template_name)
{
    // Nop
}

UIDataButton::~UIDataButton()
{
    // Nop
}

const bool UIDataButton::Visit(const std::function<bool(
    const bool,
    const std::shared_ptr<IUIData>&,
    const std::function<void(const VectorFloat2&)>&
    )>& in_visitor) const
{
    bool dirty = false;
    if (nullptr != in_visitor)
    {
        dirty = (in_visitor)(
            _enabled,
            _content,
            _on_click
            );
    }
    return dirty;
}

