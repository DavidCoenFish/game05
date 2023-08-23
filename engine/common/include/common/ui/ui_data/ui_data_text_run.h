#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class LocaleSystem;

/*
runs of text could be [string, locale key, icon, ...?]
runs of text could have style [bold, italic], font scale
*/
class UIDataTextRun : public IUIData
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

    UIDataTextRun(
        const std::vector<Data>& in_data,
        const std::string& in_template_name = std::string("UIDataTextRun")
        );
    virtual ~UIDataTextRun();

    const std::string GetString(
        LocaleSystem* const in_locale_system,
        const std::string& in_locale
        ) const;

private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    virtual const int GetChangeID() const override;

private:
    std::string _template_name;
    std::vector<Data> _data;

};
