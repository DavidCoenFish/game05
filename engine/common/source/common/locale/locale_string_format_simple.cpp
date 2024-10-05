#include "common/common_pch.h"
#include "common/locale/locale_string_format_simple.h"
#include "common/locale/locale_enum.h"

LocaleStringFormatSimple::LocaleStringFormatSimple(const std::vector<std::string>& in_data)
: _data(in_data)
{
    // nop
}

void LocaleStringFormatSimple::AccumulateString(const std::string& in_value)
{
    _result += in_value;
}

void LocaleStringFormatSimple::AccumulateToken(LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index)
{
    in_locale_system;
    in_locale;
    in_token;
    if ((0 <= in_index) && (in_index < _data.size()))
    {
        _result += _data[in_index];
    }
}
