#include "common/common_pch.h"
#include "common/util/locale_system.h"


#pragma once

class LocaleSystemImplementation
{
private:
    typedef std::map<std::string, std::string> TDataMap;
public:
    LocaleSystemImplementation()
    {
        // Nop
    }

    ~LocaleSystemImplementation()
    {
        // Nop
    }

    const std::string GetValue(
        const std::string& in_locale,
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

    const std::vector<std::string> GatherLocale() const
    {
        std::vector<std::string> result;
        for (auto iter : _locale_data_map)
        {
            result.push_back(iter.first);
        }
        return result;
    }

    void Append(
        const std::string& in_locale,
        const std::vector<LocaleSystem::Data>& in_data
        )
    {
        TDataMap* data_map = nullptr;
        auto found = _locale_data_map.find(in_locale);
        if (found != _locale_data_map.end())
        {
            auto shared_data_map = std::make_shared<TDataMap>();
            _locale_data_map[in_locale] = shared_data_map;

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
        const std::string& in_locale
        )
    {
        if (_last_locale == in_locale)
        {
            return _last_data_map;
        }
        auto found = _locale_data_map.find(in_locale);
        if (found != _locale_data_map.end())
        {
            _last_data_map = found->second.get();
            _last_locale = in_locale;
            return _last_data_map;
        }
        // if not found, don't update _last_data_map
        return nullptr;
    }

private:
    std::string _last_locale;
    TDataMap* _last_data_map;
    std::map<std::string, std::shared_ptr<TDataMap>> _locale_data_map;

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
    const std::string& in_locale,
    const std::string& in_key
    )
{
    return _implementation->GetValue(in_locale, in_key);
}

const std::vector<std::string> LocaleSystem::GatherLocale() const
{
    return _implementation->GatherLocale();
}

void LocaleSystem::Append(
    const std::string& in_locale,
    const std::vector<Data>& in_data
    )
{
    _implementation->Append(in_locale, in_data);
    return;
}
