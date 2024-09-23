#pragma once

enum class LocaleISO_639_1;

class ILocaleStringFormat
{
public:
    virtual ~ILocaleStringFormat();

    virtual const std::string GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const = 0;

    virtual void AccumulateResult(const std::string value, const LocaleISO_639_1 in_locale) = 0;
    virtual void AccumulateToken(const std::string value, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) = 0;

};
