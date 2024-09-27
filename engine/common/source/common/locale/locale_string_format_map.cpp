#include "common/common_pch.h"
#include "common/locale/locale_string_format_map.h"

LocaleStringFormatMap::LocaleStringFormatMap(const std::map<std::string, std::string>& in_data_map)
: _data_map(in_data_map)
{
    // nop
}

const std::string LocaleStringFormatMap::GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const
{
    in_locale;in_index;
    std::string result;
    const auto found = _data_map.find(in_token);
    if (found != _data_map.end())
    {
        result = found->second;
    }
    return result;
}

void LocaleStringFormatMap::AccumulateResult(const std::string value)
{
    _accumulated_result += value;
}

void LocaleStringFormatMap::AccumulateToken(const std::string value, const std::string& in_token, const int in_index)
{
    in_token; in_index;
    _accumulated_result += value;
}
