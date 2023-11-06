#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/math/vector_float2.h"

class UIDataScroll;

class UIDataManualScroll : public UIData
{
public:
    UIDataManualScroll(
        const std::shared_ptr<UIData>& in_horizontal_scroll_wrapper = nullptr,
        const std::shared_ptr<UIDataScroll>& in_horizontal_scroll = nullptr,
        const std::shared_ptr<UIData>& in_vertical_scroll_wrapper = nullptr,
        const std::shared_ptr<UIDataScroll>& in_vertical_scroll = nullptr,
        //const bool in_allow_horizontal_scroll = false, // move to factory? run time constant?
        //const bool in_allow_vertical_scroll = false,
        const std::string& in_template_name = std::string("UIDataManualScroll"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataManualScroll();

    UIData* const GetHorizontalScrollWrapper() const { return _horizontal_scroll_wrapper.get(); } 
    UIDataScroll* const GetHorizontalScroll() const { return _horizontal_scroll.get(); } 
    UIData* const GetVerticalScrollWrapper() const { return _vertical_scroll_wrapper.get(); } 
    UIDataScroll* const GetVerticalScroll() const { return _vertical_scroll.get(); } 

private:
    std::shared_ptr<UIData> _horizontal_scroll_wrapper;
    std::shared_ptr<UIDataScroll> _horizontal_scroll;
    std::shared_ptr<UIData> _vertical_scroll_wrapper;
    std::shared_ptr<UIDataScroll> _vertical_scroll;

};
