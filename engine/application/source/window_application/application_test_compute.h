#pragma once
#include "common/math/vector_float3.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class Shader;
class ShaderResource;
class UnorderedAccess;
class GeometryGeneric;
class Timer;

class ApplicationTestCompute : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationTestCompute(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestCompute();

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
        std::shared_ptr<UnorderedAccess> _compute_texture;
        std::shared_ptr<Shader> _compute_shader;

        std::shared_ptr<Shader> _present_shader;
        std::shared_ptr<GeometryGeneric> _present_geometry;
    };
    std::unique_ptr<DrawResources> _draw_resources;

};
