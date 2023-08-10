#pragma once

#include "common/ui/i_ui_content.h"

class UIContentTexture : public IUIContent
{
public:
    UIContentTexture(
        // in_source_texture
        // need a shader, different sampler, or alt shader?
        );
    virtual ~UIContentTexture();

private:
    //std::shared_ptr<ShaderResoure> _texture;
    //std::shared_ptr<Shader> _shader;
};
