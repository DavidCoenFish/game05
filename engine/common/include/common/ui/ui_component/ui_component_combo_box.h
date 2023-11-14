#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/i_ui_input.h"

class UIComponentComboBox : public IUIComponent, public IUIInput
{
    typedef IUIComponent TSuper;
public:
    typedef std::function<void(const VectorFloat4&)> TOnClick;

    UIComponentComboBox(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );
    virtual ~UIComponentComboBox();

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    virtual void OnInputClick(
        UIRootInputState& in_input_state,
        const VectorFloat4& in_screen_pos,
        const VectorFloat2& in_mouse_pos
        ) override;

private:
    //TOnClick _on_click;

    //IUIComponent* _dropdown;
    std::shared_ptr<UIHierarchyNodeChildData> _dropdown;

};
