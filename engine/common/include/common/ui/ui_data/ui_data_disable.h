#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_coord.h"

class Shader;
enum class UIEffectEnum;

class UIDataDisable : public UIData
{
public:
    UIDataDisable(
        const bool in_disabled = false,
        const std::string& in_template_name = std::string("UIDataDisable"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataDisable();

    const bool GetDisabled() const { return _disabled; }

private:
    bool _disabled;

};
