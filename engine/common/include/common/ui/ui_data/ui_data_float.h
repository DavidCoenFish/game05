#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/math/vector_float2.h"

class UIDataFloat : public UIData
{
public:
    UIDataFloat(
        const float in_value = 0.0f,
        const float in_step = 0.0f,
        const VectorFloat2& in_range_low_high = VectorFloat2(),
        const std::function<void(const float)>& in_on_value_change = nullptr,
        const std::string& in_template_name = std::string("UIDataFloat"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataFloat();

    const std::function<void(const float)>& GetOnValueChange() const { return _on_value_change; }

    const VectorFloat2& GetRangeLowHigh() const { return _range_low_high; } 

    const float GetValue() const { return _value; } 
    void SetValue(const float in_value) { _value = in_value; return; }

    const bool GetStep() const { return _step; } 

private:
    std::function<void(const bool)> _on_value_change;
    VectorFloat2 _range_low_high;
    float _value;
    float _step;

};
