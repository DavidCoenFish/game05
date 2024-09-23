#pragma once

#include "common/locale/i_locale_string_format.h"

class LocaleStringFormatLambda : public ILocaleStringFormat
{
public:
    LocaleStringFormatLambda(const std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)>& in_callback);

    const std::string& GetResult() const { return _result; }

private:
    const std::string GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const override;

    void AccumulateResult(const std::string value, const LocaleISO_639_1 in_locale) override;
    void AccumulateToken(const std::string value, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) override;

private:
    std::function<const std::string(const LocaleISO_639_1, const std::string&, const int)> _callback;
    std::string _result;

};
