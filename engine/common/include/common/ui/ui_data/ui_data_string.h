#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/locale/locale_enum.h"

class LocaleSystem;

/*
want to let the text manager what locale hint to render text as, but is it worth copying around a locale string? or move to enum
*/
class UIDataString : public UIData
{
public:
    UIDataString(
        const std::string& in_data,
        const LocaleISO_639_1 in_locale = LocaleISO_639_1::Default,
        const std::string& in_template_name = std::string("UIDataString")
        );
    virtual ~UIDataString();

    // Todo: change to visitor to future proof threading?
    const std::string& GetStringRef() const { return _data; }
    const LocaleISO_639_1 GetLocale() const { return _locale; }
    void SetString(const std::string& in_data);

private:
    std::string _data;
    LocaleISO_639_1 _locale;
    int _change_id;

};
