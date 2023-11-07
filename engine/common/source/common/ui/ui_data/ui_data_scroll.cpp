#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_scroll.h"

UIDataScroll::UIDataScroll(
    const VectorFloat2& in_value,
    const VectorFloat2& in_range_low_high,
    const std::function<void(const VectorFloat2&)>& in_on_value_change,
    const std::function<void(const VectorFloat2&)>& in_on_range_change,
    const std::shared_ptr<UIData>& in_knot_child_data,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _on_value_change(in_on_value_change)
    , _on_range_change(in_on_range_change)
    , _knot_child_data(in_knot_child_data)
    , _range_low_high(in_range_low_high)
    , _value(in_value)
{
    // Nop
}

UIDataScroll::~UIDataScroll()
{
    // Nop
}

void UIDataScroll::SetRange(const VectorFloat2& in_range)
{
    _range_low_high = in_range;
}

void UIDataScroll::SetValue(const VectorFloat2& in_value)
{
    // UIComponentScroll needs to be able to set range on the UIDataScroll, 
#if 1
    _value = in_value;
#else
    if (in_value[0] < _range_low_high[0])
    {
        _value[0] = _range_low_high[0];
    }
    else
    {
        _value[0] = in_value[0];
    }
    
    if (_range_low_high[1] < in_value[1])
    {
        _value[1] = _range_low_high[1];
    }
    else
    {
        _value[1] = in_value[1];
    }
#endif
    return;
}

