#pragma once

#include "common/ui/ui_data/ui_data.h"

class VectorFloat2;

class UIDataToggle : public UIData
{
public:
    UIDataToggle(
        const bool in_value = false,
        const std::function<void(const VectorFloat2&)>& in_on_click = nullptr,
        const std::string& in_template_name = std::string("UIDataToggle"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_true = std::vector<std::shared_ptr<UIData>>(),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_false = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataToggle();

    //const std::function<void(const bool)>& GetOnValueChange() const { return _on_value_change; }
    const std::function<void(const VectorFloat2&)>& GetOnClick() const { return _on_click; }

    const bool GetValue() const { return _value; } 
    void SetValue(const bool in_value);
    void ToggleValue();

private:
    //std::function<void(const bool)> _on_value_change;
    std::function<void(const VectorFloat2&)> _on_click;
    bool _value;
    std::vector<std::shared_ptr<UIData>> _array_child_data[2];
};
