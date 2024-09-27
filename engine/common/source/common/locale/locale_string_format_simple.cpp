#include "common/common_pch.h"
#include "common/locale/locale_string_format_simple.h"
#include "common/locale/locale_enum.h"

LocaleStringFormatSimple::LocaleStringFormatSimple(const std::vector<std::string>& in_data)
: _data(in_data)
{
    // nop
}

const std::string LocaleStringFormatSimple::GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const
{
    in_locale;
    in_token;
    if ((0 <= in_index) && (in_index < _data.size()))
    {
        return _data[in_index];
    }
    return std::string();
}

void LocaleStringFormatSimple::AccumulateResult(const std::string value)
{
    _result += value;
}

void LocaleStringFormatSimple::AccumulateToken(const std::string value, const std::string&, const int)
{
    _result += value;
}

