#pragma once
#include "common/window/i_window_application.h"

class DrawSystem;
class GeometryGeneric;
class RenderTargetTexture;
class Shader;
class ShaderConstantBuffer;
class ShaderResource;
class SceneComponentScreenQuad;
class UIManager;
class TextBlock;
class TextFont;
class TextManager;
//class UiBlock;

class ApplicationTestText : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationTestText(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestText();

private:
    virtual void Update() override;

    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr<DrawSystem> _draw_system;

    struct DrawResources
    {
        std::shared_ptr<SceneComponentScreenQuad> _screen_quad;
        std::shared_ptr<Shader> _background_shader;
        std::shared_ptr<ShaderConstantBuffer> _background_shader_constant_buffer;

        std::unique_ptr<TextManager> _text_manager;
        std::unique_ptr<TextBlock> _text_block;
    };
    std::unique_ptr<DrawResources> _draw_resources;

    int _screen_width;
    int _screen_height;

};
