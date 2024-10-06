#include "common/common_pch.h"
#include "common/locale/locale_string_format_map.h"

LocaleStringFormatMap::LocaleStringFormatMap(const std::map<std::string, std::string>& in_data_map)
: _data_map(in_data_map)
{
	// nop
}

void LocaleStringFormatMap::AccumulateString(const std::string& in_value)
{
	_accumulated_result += in_value;
}

void LocaleStringFormatMap::AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index)
{
	in_locale_system;
	in_locale;
	in_index;
	const auto found = _data_map.find(in_token);
	if (found != _data_map.end())
	{
		_accumulated_result += found->second;
	}
}
