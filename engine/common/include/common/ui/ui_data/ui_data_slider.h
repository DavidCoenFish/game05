#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/math/vector_float2.h"

/// This is the float data backing in the model for a slider
class UIDataSlider : public UIData
{
public:
    typedef std::function<void(const float)> TOnValueChange;

    /// The reason why TGetTooltip doesn't take a input param of the float value is that it is being called from the component which may not have access to the current model value 
    typedef std::function<const std::string()> TGetTooltip;

    UIDataSlider(
        const float in_value = 0.0f,
        const float in_step = 0.0f,
        const VectorFloat2& in_range_low_high = VectorFloat2(),
        const TOnValueChange& in_on_value_change = nullptr,
        const TGetTooltip& in_get_tooltip = nullptr,
        const std::shared_ptr<UIData>& in_knot_child_data = nullptr,
        const std::string& in_template_name = std::string("UIDataSlider"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataSlider();

    const TOnValueChange& GetOnValueChange() const { return _on_value_change; }

    const TGetTooltip& GetTooltip() const { return _get_tooltip; }

    const VectorFloat2& GetRangeLowHigh() const { return _range_low_high; } 

    const float GetValue() const { return _value; } 
    void SetValue(const float in_value);// { _value = in_value; return; }

    const float GetStep() const { return _step; } 
    UIData* const GetKnotChildData() const { return _knot_child_data.get(); } 
    //std::shared_ptr<UIData>& GetKnotChildDataShared() { return _knot_child_data; } 

private:
    TOnValueChange _on_value_change;
    TGetTooltip _get_tooltip;
    std::shared_ptr<UIData> _knot_child_data;
    VectorFloat2 _range_low_high;
    float _value;
    float _step;

};
