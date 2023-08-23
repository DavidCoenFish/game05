#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_string.h"

#include "common/util/locale_system.h"

UIDataString::UIDataString(
    const std::string& in_data,
    const std::string& in_template_name
    )
    : _data(in_data)
    , _template_name(in_template_name)
    , _change_id(0)
{
    // Nop
}

UIDataString::~UIDataString()
{
    // Nop
}

void UIDataString::SetString(const std::string& in_data)
{
    if (_data == in_data)
    {
        return;
    }
    _change_id += 1;
    _data = in_data;
    return;
}

const int UIDataString::GetChangeID() const
{
    return _change_id;
}
