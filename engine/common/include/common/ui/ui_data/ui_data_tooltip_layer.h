#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_enum.h"

class UIDataTextRun;
class VectorFloat2;
class VectorFloat4;

/// this is not the data for the tooltip (which is emmited from UIInput onhover as a string?)
/// this is the rules for dealing with the style/display of the tooltips
/// modify the input funtions [OnTouch, OnHover, OnClick,...] to return a tooltip string?
class UIDataTooltipLayer : public UIData
{
public:
    //constexpr int s_max_tooltip_count = 4;
    enum Variables
    {
        TMaxTooltipCount = 4
    };
    /// tooltip index, touch pos screen normalised, screen space geometry pos [-1,-1 ... 1,1] of touched lement, textrun text 
    typedef std::function<void(const int, const VectorFloat2&, const VectorFloat4&, const std::string&)> TOnTooltipChange;
    typedef std::array<std::shared_ptr<UIData>,Variables::TMaxTooltipCount> TTooltipLayoutTargetArray;
    /// in_tooltip_text_run the ID of the textrun to populate with the tooltip, under in_array_child_data. 
    ///   currently considering if the tooltip text should be pushed into in_tooltip_text_run, ie, corretly put into model
    /// in_tooltip_layout_target the ID of the component to set the position/size of the tooltip (and visiblity?), under in_array_child_data
    /// in_tooltip_scale. should we allow seperate specification of tooltip scale, this could make font size different
    explicit UIDataTooltipLayer(
        const TTooltipLayoutTargetArray& in_tooltip_layout_target_array = TTooltipLayoutTargetArray(),
        const TOnTooltipChange& in_on_tooltip_change = nullptr,
        const UITooltipType in_tooltip_type = UITooltipType::TNone,
        const float in_tooltip_scale = 1.0f, 
        const std::string& in_template_name = std::string("UIDataTooltipLayer"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataTooltipLayer();

    const UITooltipType GetTooltipType() const { return _tooltip_type; } 
    const TOnTooltipChange GetOnTooltipChange() const { return _on_tooltip_change; } 
    const TTooltipLayoutTargetArray& GetTooltipLayoutTargetArray() const { return _tooltip_layout_target_array; }
    const bool GetTooltipScale() const { return _tooltip_scale; } 

private:
    UITooltipType _tooltip_type;

    /// sending data from document to model is done via callbacks
    /// callback to to set the text for a tooltip via the model
    TOnTooltipChange _on_tooltip_change;

    /// what about multiple touches, have a fixed size array of 4 and allow max 4 tooltips? 
    TTooltipLayoutTargetArray _tooltip_layout_target_array;

    float _tooltip_scale;

};
