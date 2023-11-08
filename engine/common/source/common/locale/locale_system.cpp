#include "common/common_pch.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"

class LocaleSystemImplementation
{
private:
    typedef std::map<std::string, std::string> TDataMap;
public:
    LocaleSystemImplementation()
    : _last_locale(LocaleISO_639_1::Invalid)
    , _last_data_map(nullptr)
    {
        // Nop
    }

    ~LocaleSystemImplementation()
    {
        // Nop
    }

    const std::string GetValue(
        const LocaleISO_639_1 in_locale,
        const std::string& in_key
        )
    {
        TDataMap* const data_map = ReturnDataMap(in_locale);
        if (nullptr != data_map)
        {
            auto found = data_map->find(in_key);
            if (found != data_map->end())
            {
                return found->second;
            }
        }
        return std::string();
    }

    const std::vector<LocaleISO_639_1> GatherLocale() const
    {
        std::vector<LocaleISO_639_1> result;
        for (auto iter : _locale_data_map)
        {
            result.push_back(iter.first);
        }
        return result;
    }

    void Append(
        const LocaleISO_639_1 in_locale,
        const std::vector<LocaleSystem::Data>& in_data
        )
    {
        TDataMap* data_map = nullptr;
        auto found = _locale_data_map.find(in_locale);
        if (found == _locale_data_map.end())
        {
            auto shared_data_map = std::make_shared<TDataMap>();
            _locale_data_map[in_locale] = shared_data_map;
            data_map = shared_data_map.get();
        }
        else
        {
            data_map = found->second.get();
        }

        DSC_ASSERT(nullptr != data_map, "invalid case");

        for (const auto iter : in_data)
        {
            (*data_map)[iter._key] = iter._value;
        }
        return;
    }

private:
    TDataMap* const ReturnDataMap(
        const LocaleISO_639_1 in_locale
        )
    {
        //const LocaleISO_639_1 locale = in_locale == LocaleISO_639_1::Default ? LocaleISO_639_1::English : in_locale;
        const LocaleISO_639_1 locale = in_locale;

        if (_last_locale == locale)
        {
            return _last_data_map;
        }
        auto found = _locale_data_map.find(locale);
        if (found != _locale_data_map.end())
        {
            _last_data_map = found->second.get();
            _last_locale = locale;
            return _last_data_map;
        }
        // if not found, don't update _last_data_map
        return nullptr;
    }

private:
    LocaleISO_639_1 _last_locale;
    TDataMap* _last_data_map;
    std::map<LocaleISO_639_1, std::shared_ptr<TDataMap>> _locale_data_map;

};

LocaleSystem::LocaleSystem()
{
    _implementation = std::make_unique<LocaleSystemImplementation>();
}

LocaleSystem::~LocaleSystem()
{
    // Nop
}

const std::string LocaleSystem::GetValue(
    const LocaleISO_639_1 in_locale,
    const std::string& in_key
    )
{
    return _implementation->GetValue(in_locale, in_key);
}

const std::vector<LocaleISO_639_1> LocaleSystem::GatherLocale() const
{
    return _implementation->GatherLocale();
}

void LocaleSystem::Append(
    const LocaleISO_639_1 in_locale,
    const std::vector<Data>& in_data
    )
{
    _implementation->Append(in_locale, in_data);
    return;
}
