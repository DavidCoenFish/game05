#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_toggle.h"

UIDataToggle::UIDataToggle(
#ifdef _DEBUG
        const std::string& in_debug_name,
#endif
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const UITintColour& in_tint_colour,
        const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,

        const bool in_value,
        const TOnValueChange& in_on_click,
        const TGetTooltip& in_get_tooltip,
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_true,
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_false
    )
    : UIData(
#ifdef _DEBUG
        in_debug_name,
#endif
        in_layout,
        in_base_colour,
        in_tint_colour,
        in_array_effect_data
        )
    , _on_click(in_on_click)
    , _get_tooltip(in_get_tooltip)
    , _value(in_value)
{
    _array_child_data[0] = in_array_child_data_false;
    _array_child_data[1] = in_array_child_data_true;
}

UIDataToggle::~UIDataToggle()
{
    // Nop
}
