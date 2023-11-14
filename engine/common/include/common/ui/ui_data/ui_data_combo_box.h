#pragma once

#include "common/ui/ui_data/ui_data.h"

class UIDataListBox;
class UIDataComboBoxLayer;
class VectorFloat4;

/// What owns what item the combo box has selected, the selection index of the dropdown list box?
/// Treat combo box as a wrapper to a listbox?
class UIDataComboBox : public UIData
{
public:
    /// in_parent_selected_item: identify the data node in in_array_child_data which is to be the parent of the selected item
    UIDataComboBox(
        const std::shared_ptr<UIData>& in_parent_selected_item = nullptr,
        const std::shared_ptr<UIDataListBox>& in_dropdown_listbox = nullptr,
        const std::string& in_template_name = std::string("UIDataComboBox"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>(),
        const std::shared_ptr<UIData>& in_child_data_dropdown = nullptr,
        const std::vector<std::shared_ptr<UIData>>& in_array_item_list = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataComboBox();

    void OnSelectItem(const int in_selection_index);
    //void OnClick(const VectorFloat4& in_screen_space);

    UIData* const GetParentSelectedItem() const { return _parent_selected_item.get(); }
    UIDataListBox* const GetDropdownListbox() const { return _dropdown_listbox.get(); }
    const std::shared_ptr<UIData>& GetChildDataDropdown() const { return _child_data_dropdown; }
    //const std::vector<std::shared_ptr<UIData>>& GetArrayItemList() const { return _array_item_list; }

private:
    /// for the combo box button, what is the UIData that the selected item should be inserted under
    std::shared_ptr<UIData> _parent_selected_item;
    /// to make finding the listbox for the combo box dropdown in the _array_child_data_dropdown
    std::shared_ptr<UIDataListBox> _dropdown_listbox;
    /// full uidata hierarchy for the combo box dropdown, may include drop shadow, drift control, listbox

    std::shared_ptr<UIData> _child_data_dropdown;
    /// the list of UIData items to represent the avaliable selection, using the index from _dropdown_listbox, we select the contents to put under _parent_selected_item
    std::vector<std::shared_ptr<UIData>> _array_item_list;

    /// hold reference to combo box layer?
    //std::shared_ptr<UIDataComboBoxLayer> _combo_box_layer;

};
