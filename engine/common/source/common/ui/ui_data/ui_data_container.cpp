#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_container.h"

UIDataContainer::UIDataContainer(
    const std::vector<std::shared_ptr<IUIData>>& in_array_data,
    const std::string& in_template_name
    )
    : _change_id(0)
    , _array_data(in_array_data)
    , _template_name(in_template_name)
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

void UIDataContainer::VisitDataArray(const std::function<void(const std::vector<std::shared_ptr<IUIData>>&)>& in_visitor) const
{
    if (in_visitor)
    {
        (in_visitor)(_array_data);
    }
    return;
}
