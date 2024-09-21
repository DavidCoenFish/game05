#pragma once

class LocaleSystemImplementation;
class IStringFormatDataSource;
enum class LocaleISO_639_1;

class LocaleSystem
{
public:
    LocaleSystem();
    ~LocaleSystem();

    /// Not const to allow cache of last locale
    const std::string GetValue(
        const LocaleISO_639_1 in_locale,
        const std::string& in_key
        );

    /// example value sourced from in_key "hello {foo} may name is {}, have a nice {day}"
    /// escape via double "hello {{world}}" => "hello {world}"
    /// escape via double "hello {world}" => "hello meow" with in_stringFormatDataSource returning "meow" 
    const std::string GetValueFormatted(
        const LocaleISO_639_1 in_locale,
        const std::string& in_key,
        const IStringFormatDataSource& in_stringFormatDataSource
        );

    const std::vector<LocaleISO_639_1> GatherLocale() const;

    struct Data
    {
        std::string _key;
        std::string _value;
    };
    void Append(
        const LocaleISO_639_1 in_locale,
        const std::vector<Data>& in_data
        );

private:
    std::unique_ptr<LocaleSystemImplementation> _implementation;

};
