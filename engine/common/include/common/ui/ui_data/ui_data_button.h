#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class VectorFloat2;

class UIDataButton : public IUIData
{
public:
    UIDataButton(
        const bool in_enabled = false,
        const std::shared_ptr<IUIData>& in_content = nullptr,
        const std::function<void(const VectorFloat2&)>& in_on_click = nullptr,
        const std::string& in_template_name = std::string("UIDataButton")
        );
    virtual ~UIDataButton();

    const bool Visit(const std::function<bool(
        const bool,
        const std::shared_ptr<IUIData>&,
        const std::function<void(const VectorFloat2&)>&
        )>& in_visitor) const;

private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    //virtual const int GetChangeID() const override;

private:
    std::string _template_name;

    bool _enabled;
    std::shared_ptr<IUIData> _content;
    std::function<void(const VectorFloat2&)> _on_click;

};
