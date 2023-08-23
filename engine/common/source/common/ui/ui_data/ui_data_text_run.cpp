#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_text_run.h"

#include "common/util/locale_system.h"

 UIDataTextRun::Data::Data(
    const std::string& in_data,
    const bool in_is_locale_key
    )
    : _data(in_data)
    , _is_locale_key(in_is_locale_key)
{
    // Nop
}

UIDataTextRun::UIDataTextRun(
    const std::vector<Data>& in_data,
    const std::string& in_template_name
    )
    : _data(in_data)
    , _template_name(in_template_name)
{
    // Nop
}

UIDataTextRun::~UIDataTextRun()
{
    // Nop
}

const std::string UIDataTextRun::GetString(
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

const int UIDataTextRun::GetChangeID() const
{
    return 0;
}


