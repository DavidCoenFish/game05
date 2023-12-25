#pragma once

#include "common/ui/ui_data/ui_data.h"

class VectorFloat2;

class UIDataButton : public UIData
{
public:
    /// todo: repeat count? is the vec2 param the screen space normalised ratio of the click pos
    typedef std::function<void(const VectorFloat2&)> TOnClick;
    typedef std::function<const std::string()> TGetTooltip;

    UIDataButton(
#ifdef _DEBUG
        const std::string& in_debug_name,
#endif
        const UILayout& in_layout = UILayout::FactoryFull(),
        const UIBaseColour& in_base_colour = UIBaseColour::FactoryDefault(),
        const UITintColour& in_tint_colour = UITintColour::FactoryDefault(),
        const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data = s_empty_effect_data_array,
        UIData* const in_parent_or_null = nullptr,
        const TOnClick& in_on_click = nullptr,
        const TGetTooltip& in_get_tooltip = nullptr,
        const bool in_repeat = false
        );
    virtual ~UIDataButton();

private:
    /// Make component type match what the data wants, default is UIComponentCanvas
    /// return true if data has changed and that we need to renderer
    virtual const bool ApplyComponent(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateParam& in_param
        ) override;

private:
    TOnClick _on_click;
    TGetTooltip _get_tooltip;
    bool _repeat;

};
