#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class UIDataTemplate : public IUIData
{
public:
    UIDataTemplate(
        const std::string& in_template_name,
        const std::string& in_provider_data_key_base
        );
    virtual ~UIDataTemplate();

    const std::string GetTemplateName() const { return _template_name; }
    const std::string GetProviderDataKeyBase() const { return _provider_data_key_base; }

private:
    virtual const std::string GetClassName() const { return "UIDataTemplate"; }

private:
    std::string _template_name;
    std::string _provider_data_key_base;

};
