#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_coord.h"

class Shader;
enum class UIEffectEnum;

class UIDataEffect : public UIData
{
public:
    UIDataEffect(
        const UIEffectEnum in_type,
        const UICoord& in_coord_a = UICoord(),
        const UICoord& in_coord_b = UICoord(),
        const UICoord& in_coord_c = UICoord(),
        const UICoord& in_coord_d = UICoord(),
        const std::string& in_template_name = std::string("UIDataEffect"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataEffect();

    const UIEffectEnum GetEffectEnum() const { return _type; }

private:
    UIEffectEnum _type;
    UICoord _coord_a;
    UICoord _coord_b;
    UICoord _coord_c;
    UICoord _coord_d;

    // Shader will also take the target texture width, height, 1.0f/width, 1.0f/height
    // Shader will take a texture input from each child

};
