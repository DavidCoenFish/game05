#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_string.h"

UIDataString::UIDataString(
    const std::string& in_value
    )
    : _value(in_value)
{
    // Nop
}

UIDataString::~UIDataString()
{
    // Nop
}

const int UIDataString::GetChangeID() const
{
    return 0;
}

