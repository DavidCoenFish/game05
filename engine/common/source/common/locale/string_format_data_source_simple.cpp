#include "common/common_pch.h"
#include "common/locale/string_format_data_source_simple.h"
#include "common/locale/locale_enum.h"

StringFormatDataSourceSimple::StringFormatDataSourceSimple(const std::vector<std::string>& in_data)
: IStringFormatDataSource()
, _data(in_data)
{
    // nop
}

const std::string StringFormatDataSourceSimple::GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const
{
    in_locale;
    in_token;
    if ((0 <= in_index) && (in_index < _data.size()))
    {
        return _data[in_index];
    }
    return std::string();
}

