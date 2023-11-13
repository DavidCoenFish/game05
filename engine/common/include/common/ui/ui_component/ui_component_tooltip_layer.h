#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_data/ui_data_tooltip_layer.h"

class UIComponentTextRun;
struct TooltipRequestData;
struct UIManagerDealInputParam;

/// should tooltip position on screen be controller by layout offset or uv scroll, thinking uv scroll for smoother movement
class UIComponentTooltipLayer : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    //typedef std::function<void(const int, const VectorFloat2&, const VectorFloat4&, const std::string&)> TOnTooltipChange;

    UIComponentTooltipLayer(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );
    virtual ~UIComponentTooltipLayer();

    //void ResetTooltip();
    void RequestTooltip(
        const int in_index,
        const VectorFloat2& in_touch_pos, 
        const VectorFloat4& in_emitter_screenspace, 
        const std::string& in_text_run_string
        );

    // pass everything needed to do a late update hierarchy, update size on children
    void FinalizeTooltips(
        const std::vector<TooltipRequestData>& in_tooltip_request_data_array,
        const UIManagerDealInputParam& in_param
        );

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

private:
    /// how do we set text, tell the text run to not use text from model? or do we set the model, add OnSetTooltip callback
    UIDataTooltipLayer::TOnTooltipChange _on_tooltip_change;
    UITooltipType _tooltip_type;
    float _tooltip_scale;

    IUIComponent* _tooltip_layout_target_array[UIDataTooltipLayer::Variables::TMaxTooltipCount];

    std::map<void*, int> _map_active_tooltips;

};
