#pragma once

#include "common/ui/ui_data/i_ui_data.h"
#include "common/locale/locale_enum.h"

class LocaleSystem;

/*
want to let the text manager what locale hint to render text as, but is it worth copying around a locale string? or move to enum
*/
class UIDataString : public IUIData
{
public:
    UIDataString(
        const std::string& in_data,
        const LocaleISO_639_1 in_locale = LocaleISO_639_1::Default,
        const std::string& in_template_name = std::string("UIDataString")
        );
    virtual ~UIDataString();

    const std::string GetString() const { return _data; }
    const LocaleISO_639_1 GetLocale() const { return _locale; }
    void SetString(const std::string& in_data);

private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    virtual const int GetChangeID() const override;

private:
    std::string _template_name;
    std::string _data;
    LocaleISO_639_1 _locale;
    int _change_id;

};
