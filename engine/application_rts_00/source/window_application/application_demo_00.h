#pragma once
#include "common/window/i_window_application.h"

class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationDemo00 : public IWindowApplication
{
public:
    ApplicationDemo00(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationDemo00();
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );

private:
    virtual void Update() override;
    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr<DrawSystem> _draw_system;

};
