#pragma once

enum class LocaleISO_639_1;

class IStringFormatDataSource
{
public:
    virtual ~IStringFormatDataSource();

    virtual const std::string GetValue(const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index) const = 0;

};
