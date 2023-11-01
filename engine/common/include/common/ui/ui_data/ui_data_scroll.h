#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/math/vector_float2.h"

class UIDataScroll : public UIData
{
public:
    UIDataScroll(
        const VectorFloat2& in_value = VectorFloat2(),
        const VectorFloat2& in_range_low_high = VectorFloat2(),
        const std::function<void(const VectorFloat2&)>& in_on_value_change = nullptr,
        const std::shared_ptr<UIData>& in_knot_child_data = nullptr,
        const std::string& in_template_name = std::string("UIDataScroll"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataScroll();

    const std::function<void(const VectorFloat2&)>& GetOnValueChange() const { return _on_value_change; }

    const VectorFloat2& GetRangeLowHigh() const { return _range_low_high; } 

    const VectorFloat2& GetValue() const { return _value; } 
    void SetValue(const VectorFloat2& in_value);

    UIData* const GetKnotChildData() const { return _knot_child_data.get(); } 

private:
    std::function<void(const VectorFloat2&)> _on_value_change;
    std::shared_ptr<UIData> _knot_child_data;
    VectorFloat2 _range_low_high;
    VectorFloat2 _value;

};
