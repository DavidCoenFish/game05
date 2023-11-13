#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_slider.h"

UIDataSlider::UIDataSlider(
    const float in_value,
    const float in_step,
    const VectorFloat2& in_range_low_high,
    const TOnValueChange& in_on_value_change,
    const TGetTooltip& in_get_tooltip,
    const std::shared_ptr<UIData>& in_knot_child_data,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _on_value_change(in_on_value_change)
    , _get_tooltip(in_get_tooltip)
    , _knot_child_data(in_knot_child_data)
    , _range_low_high(in_range_low_high)
    , _value(in_value)
    , _step(in_step)
{
    // Nop
}

UIDataSlider::~UIDataSlider()
{
    // Nop
}

void UIDataSlider::SetValue(const float in_value)
{
    if (in_value < _range_low_high.GetX())
    {
        _value = _range_low_high.GetX();
    }
    else if (_range_low_high.GetY() < in_value)
    {
        _value = _range_low_high.GetY();
    }
    else
    {
        _value = in_value;
    }
    return;
}

