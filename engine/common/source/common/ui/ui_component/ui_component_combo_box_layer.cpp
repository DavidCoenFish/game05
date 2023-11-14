#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_combo_box_layer.h"
#include "common/ui/ui_data/ui_data_combo_box_layer.h"
#include "common/ui/ui_hierarchy_node.h"

UIComponentComboBoxLayer::UIComponentComboBoxLayer(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    , _active(false)
    , _insert_node(nullptr)
{
    // Nop
}

UIComponentComboBoxLayer::~UIComponentComboBoxLayer()
{
    // Nop
}

void UIComponentComboBoxLayer::TriggerComboBoxDropDown(
    const std::shared_ptr<UIHierarchyNodeChildData>& in_combo_box_dropdown
    )
{
    _combo_box_dropdown = in_combo_box_dropdown;
    _active = true;
    if (nullptr != _on_activate)
    {
        _on_activate();
    }
    return;
}

const bool UIComponentComboBoxLayer::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_parent_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;

    void* insert_dropdown_parent = nullptr;

    // turn off the combo box dropdown if no longer active from the model
    const UIDataComboBoxLayer* const data = dynamic_cast<const UIDataComboBoxLayer*>(in_data);
    if (nullptr != data)
    {
        _on_activate = data->GetOnActivate();
        if (_active != data->GetActive()) // if we set _active to true in TriggerComboBoxDropDown, then this will not catch all state changes
        {
            _active = data->GetActive();
            dirty = true;
        }
        insert_dropdown_parent = data->GetInsertDropdownParent();
    }

    if (nullptr == _insert_node)
    {
        if (true == TSuper::UpdateHierarchy(
            in_data,
            in_out_parent_child_data, 
            in_param
            ))
        {
            dirty = true;
        }

        in_out_parent_child_data.VisitComponents([this, insert_dropdown_parent](IUIComponent* const in_component, UIHierarchyNode* const in_node){
            if (insert_dropdown_parent == in_component->GetSourceToken())
            {
                _insert_node = in_node;
                return false;
            }
            return true;
        });
    }

    // add the combo box dropdown if requested/ triggerred
    if (nullptr != _combo_box_dropdown)
    {
        if (nullptr != _insert_node)
        {
            _insert_node->GetChildData().push_back(_combo_box_dropdown);
        }
        _combo_box_dropdown = nullptr;
        dirty = true;
    }

    if (nullptr != _insert_node)
    {
        if (false == _active)
        {
            _insert_node->ClearChildren();
        }
    }

    for (auto& child : in_out_parent_child_data._node->GetChildData())
    {
        IUIComponent* const component = child ? child->_component.get() : nullptr;
        if (nullptr != component)
        {
            component->SetStateFlagBit(UIStateFlag::THidden, !_active);
        }
    }

    return dirty;
}

