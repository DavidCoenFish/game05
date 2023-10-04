#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_effect.h"

UIDataEffect::UIDataEffect(
    const std::shared_ptr<Shader>& in_shader,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _shader(in_shader)
{
    return;
}

UIDataEffect::~UIDataEffect()
{
    return;
}
