#pragma once

class LocaleSystemImplementation;
enum class LocaleISO_639_1;

class LocaleSystem
{
public:
    LocaleSystem();
    ~LocaleSystem();

    const std::string GetValue(
        const LocaleISO_639_1 in_locale,
        const std::string& in_key
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
