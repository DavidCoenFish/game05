#pragma once

#include "common/ui/ui_data/ui_data.h"

class Shader;

enum class UIDataEffectType
{
    TNone,
    TDropShadow,
    TRoundCorners,
    TBlendRGBAndAlpha,
    TChromeFill
};

class UIDataEffect : public UIData
{
public:
    UIDataEffect(
        const UIDataEffectType in_type,
        //const std::shared_ptr<Shader>& in_shader,
        const std::string& in_template_name = std::string("UIDataEffect"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataEffect();

    //const std::shared_ptr<Shader>& GetShaderRef() const { return _shader; }
    const UIDataEffectType GetEffectType() const { return _type; }

private:
    //std::shared_ptr<Shader> _shader;
    UIDataEffectType _type;

};
