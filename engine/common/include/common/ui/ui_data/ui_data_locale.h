#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class LocaleSystem;

/*
well, since i keep on wanting to concatinate locale keys, set it up as an array
if too excessive to have single keys in a data object, could add a UIDataLocaleKey class?
*/
class UIDataLocale : public IUIData
{
public:
    struct Data
    {
        explicit Data(
            const std::string& in_data = std::string(),
            const bool in_is_locale_key = false
            );
        std::string _data;
        bool _is_locale_key;
    };

    UIDataLocale(
        const std::vector<Data>& in_data
        );
    virtual ~UIDataLocale();

    const std::string GetString(
        LocaleSystem* const in_locale_system,
        const std::string& in_locale
        ) const;

private:
    virtual const std::string GetTemplateName() const { return "UIDataLocale"; }

private:
    std::vector<Data> _data;

};
