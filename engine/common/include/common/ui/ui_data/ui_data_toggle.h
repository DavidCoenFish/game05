#pragma once

#include "common/ui/ui_data/ui_data.h"

class VectorFloat2;

/// Both UIDataToggle and UIDataButton reflact as UIComponentButton
class UIDataToggle : public UIData
{
    typedef UIData TSuper;
public:
    /// let the text manager what locale hint to render text as, for harfbuz
    typedef std::function<void(const VectorFloat2&)> TOnValueChange;
    typedef std::function<const std::string()> TGetTooltip;

    UIDataToggle(
#ifdef _DEBUG
        const std::string& in_debug_name,
#endif
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const UITintColour& in_tint_colour,
        const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data = s_empty_effect_data_array,
        const bool in_value = false,
        const TOnValueChange& in_on_click = nullptr,
        const TGetTooltip& in_get_tooltip = nullptr,
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_true = std::vector<std::shared_ptr<UIData>>(),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data_false = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataToggle();

    const bool GetValue() const { return _value; }

private:
    virtual const bool ApplyComponent(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateParam& in_param
        ) override;

private:
    TOnValueChange _on_click;
    TGetTooltip _get_tooltip;
    bool _value;
    std::vector<std::shared_ptr<UIData>> _array_child_data[2];

};
