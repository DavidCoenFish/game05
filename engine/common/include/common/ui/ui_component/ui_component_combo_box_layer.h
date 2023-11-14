#pragma once

#include "common/ui/ui_component/i_ui_component.h"

class UIComponentComboBoxLayer : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    //typedef std::function<void(const std::vector<std::shared_ptr<UIData>>&)> TTriggerComboBoxDropDown;

    UIComponentComboBoxLayer(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );
    virtual ~UIComponentComboBoxLayer();

    // or is all this done in the background of the model away from the components, ie, the combobox on click does the work of putting data into the UIComponentComboBoxLayer
    /// how do we get the _array_child_data_dropdown back into the model, did have callbacks for modifying the model, is that the same case here
    void TriggerComboBoxDropDown(
        const std::shared_ptr<UIHierarchyNodeChildData>& in_combo_box_dropdown
        );

private:
    virtual const bool UpdateHierarchy(
        UIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

private:
    // there are two paths to deactivation, which both may be expressed via the model, clicking outside the dropdown, or selecting something in the dropdown
    /// tell the model that a combo box has been activated/ triggered
    std::function<void()> _on_activate;
    bool _active; // not using state flag hidden, as that will block UpdateHierarchy?

    /// or do we pass a std::shared_ptr<UIData> _dropdown_data?
    std::shared_ptr<UIHierarchyNodeChildData> _combo_box_dropdown;

    UIHierarchyNode* _insert_node;

};
