#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_template.h"

UIDataTemplate::UIDataTemplate(
    const std::string& in_template_name,
    const std::string& in_provider_data_key_base
    )
    : _template_name(in_template_name)
    , _provider_data_key_base(in_provider_data_key_base)
{
    return;
}

UIDataTemplate::~UIDataTemplate()
{
    return;
}

const int UIDataTemplate::GetChangeID() const
{
    return 0;
}

