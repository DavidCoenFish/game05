#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_coord.h"

class Shader;

enum class UIDataEffectType
{
    TNone,
    TDropShadow,
    TRoundCorners,
    TDesaturate,
    TBlendRGBAndAlpha // Use child 0 as rgb, and child 1 as alpha ~ take average of rgb to use as alpha
    // TBlend is not needed, the default shader will do a premultiplied alpha blend of the children
    // TChromeFill move fill to a different element
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
    UICoord _coord_a;
    UICoord _coord_b;
    UICoord _coord_c;
    UICoord _coord_d;
    // VectorFloat4 _tint; or use base tint

    // Shader will also take the target texture width, height, 1.0f/width, 1.0f/height
    // Shader will take a texture input from each child

};
