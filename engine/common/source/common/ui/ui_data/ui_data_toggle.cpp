#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_toggle.h"

UIDataToggle::UIDataToggle(
    const bool in_value,
    const std::function<void(const VectorFloat2&)>& in_on_click,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data_true,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data_false
    )
    : UIData(
        in_template_name,
        in_value ? in_array_child_data_true : in_array_child_data_false
        )
    , _on_click(in_on_click)
    , _value(in_value)
{
    _array_child_data[0] = in_array_child_data_false;
    _array_child_data[1] = in_array_child_data_true;
}

UIDataToggle::~UIDataToggle()
{
    // Nop
}

void UIDataToggle::SetValue(const bool in_value)
{
    if (_value == in_value)
    {
        return;
    }
    _value = in_value;
    ModifyData() = _array_child_data[static_cast<int>(_value)];
    return;
}

void UIDataToggle::ToggleValue()
{
    SetValue(!_value);
    return;
}
