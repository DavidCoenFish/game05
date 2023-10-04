#pragma once

#include "common/ui/ui_data/ui_data.h"

class VectorFloat2;

class UIDataButton : public UIData
{
public:
    UIDataButton(
        const std::function<void(const VectorFloat2&)>& in_on_click = nullptr,
        const bool in_enabled = false,
        const std::string& in_template_name = std::string("UIDataButton"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataButton();

    const bool GetEnabled() const { return _enabled; }
    const std::function<void(const VectorFloat2&)>& GetOnClick() const { return _on_click; }

private:
    bool _enabled;
    std::function<void(const VectorFloat2&)> _on_click;

};
