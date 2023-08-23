#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class LocaleSystem;

class UIDataString : public IUIData
{
public:
    UIDataString(
        const std::string& in_data,
        const std::string& in_template_name = std::string("UIDataString")
        );
    virtual ~UIDataString();

    const std::string GetString() const { return _data; }
    void SetString(const std::string& in_data);

private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    virtual const int GetChangeID() const override;

private:
    std::string _template_name;
    std::string _data;
    int _change_id;

};
