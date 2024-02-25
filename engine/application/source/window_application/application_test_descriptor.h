#pragma once
#include "common/math/vector_float3.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class SceneComponentScreenQuad;
class Shader;
class ShaderResource;
class TextManager;

class ApplicationTestDescriptor : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationTestDescriptor(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestDescriptor();

private:
    virtual void Update() override;
    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr<DrawSystem> _draw_system;

    struct DrawResource
    {
        std::shared_ptr<SceneComponentScreenQuad> _screen_quad;

        std::shared_ptr<Shader> _shader;
        std::shared_ptr<ShaderResource> _texture_0;
        std::shared_ptr<ShaderResource> _texture_1;
        std::shared_ptr<ShaderResource> _texture_2;
        std::shared_ptr<ShaderResource> _texture_3;
        std::shared_ptr<ShaderResource> _texture_4;

        std::unique_ptr<TextManager> _text_manager;
    };
    std::unique_ptr<DrawResource> _draw_resource;

};
