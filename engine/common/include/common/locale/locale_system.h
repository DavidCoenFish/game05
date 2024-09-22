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
    /// "hello {world}" => "hello meow" with in_stringFormatDataSource returning "meow" for token "world"
    /// collection of result moved into IStringFormatDataSource to allow tooltip to build link data
    void GetValueFormatted(
        const LocaleISO_639_1 in_locale,
        const std::string& in_key,
        IStringFormatDataSource& in_stringFormatDataSource
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
