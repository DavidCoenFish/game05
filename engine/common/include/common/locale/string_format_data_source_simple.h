#pragma once

#include "common/locale/i_string_format_data_source.h"

class StringFormatDataSourceSimple : public IStringFormatDataSource
{
public:
    StringFormatDataSourceSimple(const std::vector<std::string>& in_data);

    const std::string& GetResult() const { return _result; }

private:
    const std::string GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const override;

    void AccumulateResult(const std::string value, const LocaleISO_639_1 in_locale) override;
    void AccumulateToken(const std::string value, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) override;

private:
    std::vector<std::string> _data;
    std::string _result;
    
};
