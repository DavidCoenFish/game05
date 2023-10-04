#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_effect.h"

UIDataEffect::UIDataEffect(
    //const std::shared_ptr<Shader>& in_shader,
    const UIDataEffectType in_type,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    //, _shader(in_shader)
    , _type(in_type)
{
    return;
}

UIDataEffect::~UIDataEffect()
{
    return;
}
