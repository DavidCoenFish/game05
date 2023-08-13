#pragma once

class LocaleSystemImplementation;

class LocaleSystem
{
public:
    LocaleSystem();
    ~LocaleSystem();

    const std::string& GetValue(
        const std::string& in_locale,
        const std::string& in_key
        );

    const std::vector<std::string> GatherLocale() const;

    struct Data
    {
        std::string _key;
        std::string _value;
    };
    void Append(
        const std::string& in_locale,
        const std::vector<Data>& in_data
        );

private:
    std::unique_ptr<LocaleSystemImplementation> _implementation;

};
