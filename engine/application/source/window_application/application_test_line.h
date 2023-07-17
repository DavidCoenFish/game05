#pragma once
#include "common/window/i_window_application.h"

class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationTestLine : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationTestLine(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestLine();

private:
    virtual void Update() override;
    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr < DrawSystem > _draw_system;
    std::shared_ptr < Shader > _shader;
    std::shared_ptr < GeometryGeneric > _geometry;
};
