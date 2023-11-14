#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_combo_box.h"

#include "common/math/vector_float4.h"
#include "common/ui/ui_data/ui_data_list_box.h"
#include "common/ui/ui_data/ui_data_combo_box_layer.h"

UIDataComboBox::UIDataComboBox(
    const std::shared_ptr<UIData>& in_parent_selected_item,
    const std::shared_ptr<UIDataListBox>& in_dropdown_listbox,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data,
    const std::shared_ptr<UIData>& in_child_data_dropdown,
    const std::vector<std::shared_ptr<UIData>>& in_array_item_list
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _parent_selected_item(in_parent_selected_item)
    , _dropdown_listbox(in_dropdown_listbox)
    , _child_data_dropdown(in_child_data_dropdown)
    , _array_item_list(in_array_item_list)
{
    // Nop
}

UIDataComboBox::~UIDataComboBox()
{
    // Nop
}

void UIDataComboBox::OnSelectItem(const int in_selection_index)
{
    if ((in_selection_index < 0) || (_array_item_list.size() <= in_selection_index))
    {
        return;
    }

    if (nullptr != _parent_selected_item)
    {
        _parent_selected_item->ModifyData() = std::vector<std::shared_ptr<UIData>>({
            _array_item_list[in_selection_index]
            });
    }

    // not sure if this function will be driven by the dropdown listbox or other
    // we also have keyboard navigation which may change selection and need to keep the _dropdown_listbox in sync...
    if (nullptr != _dropdown_listbox)
    {
        _dropdown_listbox->SetSelectedIndex(in_selection_index);
    }

    return;
}
