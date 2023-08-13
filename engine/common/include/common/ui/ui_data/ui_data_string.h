#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class UIDataString : public IUIData
{
public:
    UIDataString(
        const std::string& in_key
        );
    virtual ~UIDataString();

    const std::string GetKey() const { return _key; }

private:
    virtual const std::string GetClassName() const { return "UIDataString"; }

private:
    std::string _key;

};
