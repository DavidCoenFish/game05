#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_disable.h"

UIDataDisable::UIDataDisable(
    const bool in_disabled,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _disabled(in_disabled)
{
    // Nop
}

UIDataDisable::~UIDataDisable()
{
    // Nop
}

