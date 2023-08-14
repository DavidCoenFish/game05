#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_locale.h"

#include "common/util/locale_system.h"

 UIDataLocale::Data::Data(
    const std::string& in_data,
    const bool in_is_locale_key
    )
    : _data(in_data)
    , _is_locale_key(in_is_locale_key)
{
    // Nop
}

UIDataLocale::UIDataLocale(
    const std::vector<Data>& in_data
    )
    : _data(in_data)
{
    // Nop
}

UIDataLocale::~UIDataLocale()
{
    // Nop
}

const std::string UIDataLocale::GetString(
    LocaleSystem* const in_locale_system,
    const std::string& in_locale
    ) const
{
    std::string result;
    for (const auto& iter : _data)
    {
        if (true == iter._is_locale_key)
        {
            result += in_locale_system->GetValue(in_locale, iter._data);
        }
        else
        {
            result += iter._data;
        }
    }
    return result;
}