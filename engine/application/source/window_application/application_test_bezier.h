#pragma once
#include "common/math/vector_float3.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class Shader;
class ShaderConstantBuffer;
class GeometryGeneric;
class Timer;

class ApplicationTestBezier : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationTestBezier(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationTestBezier();

private:
    virtual void Update() override;
    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;
    virtual void OnKey(
        const int in_vk_code, 
        const int in_scan_code, 
        const bool in_repeat_flag, 
        const int in_repeat_count, 
        bool in_up_flag
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr<DrawSystem> _draw_system;
    std::shared_ptr<GeometryGeneric> _geometry;
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

};
