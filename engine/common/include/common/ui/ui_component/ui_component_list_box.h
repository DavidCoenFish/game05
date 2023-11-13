#pragma once

#include "common/ui/ui_component/i_ui_component.h"


class UIComponentListBox : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentListBox(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );
    virtual ~UIComponentListBox();

    const bool SetBase(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );

    /// inpoint for item button callbacks
    void SetSelectedIndex(const int in_selected_index);

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

private:
    std::function<void(const int)> _on_selection_change;

    // moving away from having a shadow copy of state in ui components, do need the on change callback though
    // coudl be usefull to filter out non change input?
    //int _selection_index;

};
