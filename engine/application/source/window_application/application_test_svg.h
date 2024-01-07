#pragma once
#include "common/window/i_window_application.h"

class DrawSystem;
class GeometryGeneric;
class Shader;
class ShaderResource;

class ApplicationTestSvg : public IWindowApplication
{
public:
    ApplicationTestSvg(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestSvg();
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
    std::shared_ptr<ShaderResource> _shader_resource;
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<GeometryGeneric> _geometry;

};
