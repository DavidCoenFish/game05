#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_manual_scroll.h"

UIDataManualScroll::UIDataManualScroll(
    const std::shared_ptr<UIData>& in_horizontal_scroll_wrapper,
    const std::shared_ptr<UIDataScroll>& in_horizontal_scroll,
    const std::shared_ptr<UIData>& in_vertical_scroll_wrapper,
    const std::shared_ptr<UIDataScroll>& in_vertical_scroll,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _horizontal_scroll_wrapper(in_horizontal_scroll_wrapper)
    , _horizontal_scroll(in_horizontal_scroll)
    , _vertical_scroll_wrapper(in_vertical_scroll_wrapper)
    , _vertical_scroll(in_vertical_scroll)
{
    // Nop
}

UIDataManualScroll::~UIDataManualScroll()
{
    // Nop
}
