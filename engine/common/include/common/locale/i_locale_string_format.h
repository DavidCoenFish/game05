#pragma once

enum class LocaleISO_639_1;
class LocaleSystem;

class ILocaleStringFormat
{
public:
    virtual ~ILocaleStringFormat();

    virtual void AccumulateString(const std::string& in_value) = 0;
    /// in_index is the trace cound of which token this is from in the format template "so {a} {b} {c}", not ver safe as different locale could have different orders
    virtual void AccumulateToken(LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) = 0;

};
