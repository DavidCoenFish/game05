#pragma once

enum class LocaleISO_639_1;
class LocaleSystem;
class ILocaleDataProvider
{
public:
    virtual ~ILocaleDataProvider();

    // add data to the locale system for the given locale
    virtual void Populate(LocaleSystem& in_out_local_system, const LocaleISO_639_1 in_locale) const = 0;

};
