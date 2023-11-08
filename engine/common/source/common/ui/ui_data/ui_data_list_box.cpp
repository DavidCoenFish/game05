#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_list_box.h"

UIDataListBox::UIDataListBox(
    const std::function<void(const int)>& in_on_select_index_change,
    const int in_selected_index,
    const std::vector<std::shared_ptr<UIDataButton>>& in_item_host_button_array,
    //const std::vector<std::shared_ptr<UIData>>& in_item_array,
    //const std::shared_ptr<UIData>& in_container, // what in our in_array_child_data is the container
    //const std::shared_ptr<UIData>& in_item_host_template, // put item data contents at bottom?
    //const std::shared_ptr<UIDataButton>& in_item_host_button, // what in in_item_host_template is the button
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _on_select_index_change(in_on_select_index_change)
    , _selected_index(in_selected_index)
    , _item_host_button_array(in_item_host_button_array)
    //, _item_array(in_item_array)
    //, _container(in_container)
    //, _item_host_template(in_item_host_template)
    //, _item_host_button(in_item_host_button)
{
    // Nop
}

UIDataListBox::~UIDataListBox()
{
    // Nop
}
