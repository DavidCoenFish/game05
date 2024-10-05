#pragma once

#include "common/locale/i_locale_string_format.h"

class LocaleSystem;

class LocaleStringFormatMap : public ILocaleStringFormat
{
public:
    LocaleStringFormatMap(const std::map<std::string, std::string>& in_data_map);
    const std::string& GetResult() const { return _accumulated_result; }
private:
    void AccumulateString(const std::string& in_value) override;
    void AccumulateToken(LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) override;

private:
    std::map<std::string, std::string> _data_map; 
    std::string _accumulated_result;
};
