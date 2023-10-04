#pragma once

#include "common/ui/ui_data/ui_data.h"

class Shader;

class UIDataEffect : public UIData
{
public:
    UIDataEffect(
        const std::shared_ptr<Shader>& in_shader,
        const std::string& in_template_name = std::string("UIDataEffect"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataEffect();

    const std::shared_ptr<Shader>& GetShaderRef() const { return _shader; }

private:
    std::shared_ptr<Shader> _shader;

};
