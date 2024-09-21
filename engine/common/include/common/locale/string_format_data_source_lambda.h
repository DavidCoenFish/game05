#pragma once

#include "common/locale/i_string_format_data_source.h"

class StringFormatDataSourceLambda : public IStringFormatDataSource
{
public:
    StringFormatDataSourceLambda(const std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)>& in_callback);

private:
    const std::string GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const override;

private:
    std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)> _callback;

};
