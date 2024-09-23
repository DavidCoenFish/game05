#include "common/common_pch.h"
#include "common/locale/locale_string_format_lambda.h"
#include "common/locale/locale_enum.h"

LocaleStringFormatLambda::LocaleStringFormatLambda(const std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)>& in_callback)
: _callback(in_callback)
{
    // nop
}

const std::string LocaleStringFormatLambda::GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const
{
    if (_callback)
    {
        return _callback(in_locale, in_token, in_index);
    }
    return std::string();
}

void LocaleStringFormatLambda::AccumulateResult(const std::string value, const LocaleISO_639_1)
{
    _result += value;
}

void LocaleStringFormatLambda::AccumulateToken(const std::string value, const LocaleISO_639_1, const std::string&, const int)
{
    _result += value;
}

