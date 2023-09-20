#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class LocaleSystem;
enum class LocaleISO_639_1;

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

    //const std::string GetString(
    //    LocaleSystem* const in_locale_system,
    //    const LocaleISO_639_1 in_locale
    //    ) const;

    virtual void VisitDataArray(
        const std::function<void(const std::vector<Data>&)>& in_visitor
        ) const = 0;


private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    virtual const int GetChangeID() const override;

private:
    std::string _template_name;
    std::vector<Data> _data;

};
