#pragma once

#include "common/locale/i_locale_string_format.h"

class LocaleStringFormatMap : public ILocaleStringFormat
{
public:
    LocaleStringFormatMap(const std::map<std::string, std::string>& in_data_map);
    const std::string& GetResult() const { return _accumulated_result; }
private:
    const std::string GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const override;
    void AccumulateResult(const std::string value) override;
    void AccumulateToken(const std::string value, const std::string& in_token, const int in_index) override;

private:
    std::map<std::string, std::string> _data_map; 
    std::string _accumulated_result;
};
