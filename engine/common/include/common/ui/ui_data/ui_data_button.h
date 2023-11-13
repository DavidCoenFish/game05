#pragma once

#include "common/ui/ui_data/ui_data.h"

class VectorFloat2;

class UIDataButton : public UIData
{
public:
    typedef std::function<void(const VectorFloat2&)> TOnValueChange;
    typedef std::function<const std::string()> TGetTooltip;

    UIDataButton(
        const TOnValueChange& in_on_click = nullptr,
        const TGetTooltip& in_get_tooltip = nullptr,
        const bool in_repeat = false,
        const std::string& in_template_name = std::string("UIDataButton"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataButton();

    const TOnValueChange& GetOnClick() const { return _on_click; }
    const TGetTooltip& GetTooltip() const { return _get_tooltip; }

    const bool GetRepeat() const { return _repeat; }

private:
    TOnValueChange _on_click;
    TGetTooltip _get_tooltip;
    bool _repeat;

};
