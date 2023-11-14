#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_combo_box.h"

#include "common/ui/ui_root_input_state.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_data/ui_data_combo_box.h"
#include "common/ui/ui_data/ui_data_combo_box_layer.h"

UIComponentComboBox::UIComponentComboBox(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
{
    // Nop
}

UIComponentComboBox::~UIComponentComboBox()
{
    // Nop
}

const bool UIComponentComboBox::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    const UIDataComboBox* const data = dynamic_cast<UIDataComboBox*>(in_data);
    if (nullptr != data)
    {
        std::shared_ptr<UIData> child_data_dropdown = data->GetChildDataDropdown();
        //std::shared_ptr<UIHierarchyNodeChildData> _dropdown;
        if (nullptr == _dropdown)
        {
            _dropdown = UIHierarchyNodeChildData::Factory();
        }
        _dropdown->ApplyFactory(child_data_dropdown.get(), in_param);
    }

    if (true == TSuper::UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        ))
    {
        dirty = true;
    }

    return dirty;
}

void UIComponentComboBox::OnInputClick(
    UIRootInputState& in_input_state,
    const VectorFloat4& in_screen_pos,
    const VectorFloat2&
    )
{
    //if (nullptr != _on_click)
    //{
    //    _on_click(in_screen_pos);
    //}

    IUIComponent* const component = _dropdown ? _dropdown->_component.get() : nullptr;
    if (nullptr != component)
    {
        UILayout layout = component->GetLayout();

        layout.SetComboBoxDropdown(in_screen_pos);

        component->SetLayoutOverride(layout);
    }

    in_input_state.RequestComboBoxDropdown(_dropdown);

    // do we clear _dropdown here and make a new one in the update, if there are "one off" components like drift....
    _dropdown.reset();

    return;
}

