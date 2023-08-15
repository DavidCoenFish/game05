#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class UIDataString : public IUIData
{
public:
    UIDataString(
        const std::string& in_value
        );
    virtual ~UIDataString();

    const std::string GetKey() const { return _value; }

private:
    virtual const std::string GetTemplateName() const { return "UIDataString"; }

private:
    std::string _value;

};
