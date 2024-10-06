#include "common/common_pch.h"
#include "common/locale/locale_string_format_lambda.h"
#include "common/locale/locale_enum.h"

LocaleStringFormatLambda::LocaleStringFormatLambda(const std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)>& in_callback)
: _callback(in_callback)
{
	// nop
}

void LocaleStringFormatLambda::AccumulateString(const std::string& in_value)
{
	_result += in_value;
}
void LocaleStringFormatLambda::AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index)
{
	in_locale_system;
	if (_callback)
	{
		_result += _callback(in_locale, in_token, in_index);
	}
}
