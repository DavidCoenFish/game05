#pragma once

#include "common/ui/ui_data/ui_data.h"

class UIDataButton;

/// Rather than having the list box act as a factory, move that upstream, and just have the render and input nodes ready for consumption at this level
/// ie, UIDataListBox has the list of UIButtonData that hosts items internal to in_array_child_data
/// and there is a magic factory upstream that can convert a set of items into an in_array_child_data and in_item_host_button_array
class UIDataListBox : public UIData
{
public:
    UIDataListBox(
        const std::function<void(const int)>& in_on_select_index_change = nullptr,
        const int in_selected_index = 0,
        const std::vector<std::shared_ptr<UIDataButton>>& in_item_host_button_array = std::vector<std::shared_ptr<UIDataButton>>(),
        //const std::vector<std::shared_ptr<UIData>>& in_item_array = std::vector<std::shared_ptr<UIData>>(),
        //const std::shared_ptr<UIData>& in_container = nullptr, // what in our in_array_child_data is the container
        //const std::shared_ptr<UIData>& in_item_host_template = nullptr, // put item data contents at bottom?
        //const std::shared_ptr<UIDataButton>& in_item_host_button = nullptr, // what in in_item_host_template is the button
        const std::string& in_template_name = std::string("UIDataListBox"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataListBox();

    const std::function<void(const int)>& GetOnSelectedIndexChange() const { return _on_select_index_change; } 
    const int GetSelectedIndex() const { return _selected_index; } 
    //UIData* const GetContainer() const { return _container.get(); } 
    //UIData* const GetItemHostTemplate() const { return _item_host_template.get(); } 
    //UIDataButton* const GetItemHostButton() const { return _item_host_button.get(); } 

    /// push more things through using callback for setting index, _on_select_index_change
    //void SetSelectedIndex(const float in_selected_index);// { _selected_index = in_selected_index; return; }

    const std::vector<std::shared_ptr<UIDataButton>>& GetItemHostButtonArrayRef() const { return _item_host_button_array; }

private:
    /// the data for the node somewhere in our _array_child_data that will host all the items
    //std::shared_ptr<UIData> _container;

    //std::vector<std::shared_ptr<UIData>> _item_array;
    //std::shared_ptr<UIData> _container;
    //std::shared_ptr<UIData> _item_host_template;
    //std::shared_ptr<UIDataButton> _item_host_button;

    /// Reduce load on UIDataListBox by having the data fit render list design better, 
    /// The array of DataButtons that host items, these dataButtion are expected to be under our _array_child_data
    /// Order of _item_host_button_array determins order of selection
    std::vector<std::shared_ptr<UIDataButton>> _item_host_button_array;

    /// use of callback is to pass to UIComponent so it has something to call, felt bad with holding a reference to a UIData*
    /// dont want to call callback from the SetSelectedIndex, as the callback may call the SetSelectedIndex?
    std::function<void(const int)> _on_select_index_change;
    int _selected_index;

};
