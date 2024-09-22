#include "common/common_pch.h"
#include "common/locale/string_format_data_source_lambda.h"
#include "common/locale/locale_enum.h"

StringFormatDataSourceLambda::StringFormatDataSourceLambda(const std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)>& in_callback)
: IStringFormatDataSource()
, _callback(in_callback)
{
    // nop
}

const std::string StringFormatDataSourceLambda::GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const
{
    if (_callback)
    {
        return _callback(in_locale, in_token, in_index);
    }
    return std::string();
}

void StringFormatDataSourceLambda::AccumulateResult(const std::string value, const LocaleISO_639_1)
{
    _result += value;
}

void StringFormatDataSourceLambda::AccumulateToken(const std::string value, const LocaleISO_639_1, const std::string&, const int)
{
    _result += value;
}

