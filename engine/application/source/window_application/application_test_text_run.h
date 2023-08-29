#pragma once
#include "common/window/i_window_application.h"

class DrawSystem;
class GeometryGeneric;
class RenderTargetTexture;
class Shader;
class ShaderResource;
class SceneComponentScreenQuad;
class UIManager;
class TextRun;
class TextFont;
class TextManager;
//class UiBlock;

class ApplicationTestTextRun : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationTestTextRun(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestTextRun();

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

        std::unique_ptr<TextManager> _text_manager;
        std::unique_ptr<TextRun> _text_run;
    };
    std::unique_ptr<DrawResources> _draw_resources;

    int _screen_width;
    int _screen_height;

};
