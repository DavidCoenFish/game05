#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_tooltip_layer.h"
#include "common/ui/ui_data/ui_data_text_run.h"

UIDataTooltipLayer::UIDataTooltipLayer(
    const TTooltipLayoutTargetArray& in_tooltip_layout_target_array,
    const TOnTooltipChange& in_on_tooltip_change,
    const UITooltipType in_tooltip_type,
    const float in_tooltip_scale, 
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _tooltip_layout_target_array(in_tooltip_layout_target_array)
    , _on_tooltip_change(in_on_tooltip_change)
    , _tooltip_type(in_tooltip_type)
    , _tooltip_scale(in_tooltip_scale)
{
    // Nop
}

UIDataTooltipLayer::~UIDataTooltipLayer()
{
    // Nop
}
