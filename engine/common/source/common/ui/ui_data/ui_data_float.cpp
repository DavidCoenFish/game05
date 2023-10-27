#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_float.h"

UIDataFloat::UIDataFloat(
    const float in_value,
    const float in_step,
    const VectorFloat2& in_range_low_high,
    const std::function<void(const float)>& in_on_value_change,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _on_value_change(in_on_value_change)
    , _range_low_high(in_range_low_high)
    , _value(in_value)
    , _step(in_step)
{
    // Nop
}

UIDataFloat::~UIDataFloat()
{
    // Nop
}
