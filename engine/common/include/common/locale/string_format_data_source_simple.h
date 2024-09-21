#pragma once

#include "common/locale/i_string_format_data_source.h"

class StringFormatDataSourceSimple : public IStringFormatDataSource
{
public:
    StringFormatDataSourceSimple(const std::vector<std::string>& in_data);

private:
    const std::string GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const override;

private:
    std::vector<std::string> _data;
    
};
