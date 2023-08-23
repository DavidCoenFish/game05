#pragma once

#include "common/ui/ui_data/ui_data_button.h"

class UIDataIconButton : public UIDataButton
{
public:
    UIDataIconButton(
        const std::string& in_icon_key,
        const bool in_enabled,
        const std::string& in_label_key,
        const std::function<void(UIDataButton*)>& in_on_click,
        const std::function<void(UIDataButton*)>& in_on_focus,
        const std::string& in_template_name = std::string("UIDataIconButton")
    );
    virtual ~UIDataIconButton();

    const std::string GetIconKey() const { return _icon_key; }

private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    virtual const int GetChangeID() const override;

private:
    std::string _template_name;
    std::string _icon_key;

};
