#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_container.h"

UIDataContainer::UIDataContainer(
    const std::vector<std::shared_ptr<IUIData>>& in_array_data
    )
    : _change_id(0)
    , _array_data(in_array_data)
{
    // Nop
}

UIDataContainer::~UIDataContainer()
{
    // Nop
}

std::vector<std::shared_ptr<IUIData>>& UIDataContainer::ModifyData()
{
    _change_id += 1;
    return _array_data;
}

const int UIDataContainer::GetChangeID() const
{
    return _change_id;
}

