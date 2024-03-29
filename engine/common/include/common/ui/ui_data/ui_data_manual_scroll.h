#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/math/vector_float2.h"

class UIDataScroll;

/// The manual scroll is metadata on the provided normal data hierarchy
/// todo: provide alternative uilayout for in_array_child_data[0] for when horizontal/ vertical scrolls are active? 
class UIDataManualScroll : public UIData
{
public:
    /// in_horizontal_scroll is the ID/UIData of the scroll somewhere under in_array_child_data, or null 
    /// in_vertical_scroll is the ID/UIData of the scroll somewhere under in_array_child_data, or null
    /// in_array_child_data[0] is the contents 
    /// in_array_child_data[1] is the horizontal scroll wrapper or null
    /// in_array_child_data[2] is the vertical scroll wrapper or null
    UIDataManualScroll(
        const std::shared_ptr<UIDataScroll>& in_horizontal_scroll = nullptr,
        const std::shared_ptr<UIDataScroll>& in_vertical_scroll = nullptr,
        const std::string& in_template_name = std::string("UIDataManualScroll"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        //, const UILayout[4] in_alternative_content_layout 
        );
    virtual ~UIDataManualScroll();

    UIDataScroll* const GetHorizontalScroll() const { return _horizontal_scroll.get(); } 
    UIDataScroll* const GetVerticalScroll() const { return _vertical_scroll.get(); } 

private:
    /// the ID/UIData of the scroll somewhere under in_array_child_data[1] and [2]
    std::shared_ptr<UIDataScroll> _horizontal_scroll;
    std::shared_ptr<UIDataScroll> _vertical_scroll;

};
