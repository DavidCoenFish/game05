#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class UIDataButton : public IUIData
{
public:
    UIDataButton(
        const bool in_enabled,
        const std::string& in_label_key,
        const std::function<void(UIDataButton*)>& in_on_click,
        const std::function<void(UIDataButton*)>& in_on_focus,
        const std::string& in_template_name = std::string("UIDataButton")
        );
    virtual ~UIDataButton();

    const std::string GetLabelKey() const { return _label_key; }
    const bool GetEnabled() const { return _enabled; }

    void OnClick();
    void OnFocus();

private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    virtual const int GetChangeID() const override;

private:
    std::string _template_name;
    bool _enabled;
    std::string _label_key;
    // TODO: or make virtual
    std::function<void(UIDataButton*)> _on_click;
    std::function<void(UIDataButton*)> _on_focus;

};
