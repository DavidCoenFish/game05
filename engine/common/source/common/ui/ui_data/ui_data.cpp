#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data.h"

UIData::UIData(
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : _template_name(in_template_name)
    , _array_child_data(in_array_child_data)
{
    // Nop
}

UIData::~UIData()
{
    // Nop
}

const bool UIData::VisitDataArray(const std::function<bool(const std::vector<std::shared_ptr<UIData>>&)>& in_visitor) const
{
    bool dirty = false;
    if (in_visitor)
    {
        dirty = (in_visitor)(_array_child_data);
    }

    return dirty;
}

std::vector<std::shared_ptr<UIData>>& UIData::ModifyData()
{
    //_change_id += 1;
    return _array_child_data;
}

const std::string& UIData::GetTemplateName() const
{
    return _template_name;
}

