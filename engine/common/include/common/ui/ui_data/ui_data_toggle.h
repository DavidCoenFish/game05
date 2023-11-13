#pragma once

#include "common/ui/ui_data/ui_data.h"

class VectorFloat2;

/// Both UIDataToggle and UIDataButton reflact as UIComponentButton
class UIDataToggle : public UIData
{
public:
    typedef std::function<void(const VectorFloat2&)> TOnValueChange;
    typedef std::function<const std::string()> TGetTooltip;

    UIDataToggle(
        const bool in_value = false,
        const TOnValueChange& in_on_click = nullptr,
        const TGetTooltip& in_get_tooltip = nullptr,
        const std::string& in_template_name = std::string("UIDataToggle"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_true = std::vector<std::shared_ptr<UIData>>(),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_false = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataToggle();

    const TOnValueChange& GetOnClick() const { return _on_click; }
    const TGetTooltip& GetTooltip() const { return _get_tooltip; }

    const bool GetValue() const { return _value; } 
    void SetValue(const bool in_value);
    void ToggleValue();

private:
    TOnValueChange _on_click;
    TGetTooltip _get_tooltip;
    bool _value;
    std::vector<std::shared_ptr<UIData>> _array_child_data[2];
};
