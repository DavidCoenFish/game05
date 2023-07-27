#pragma once
#include "common/math/vector_float3.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class Shader;
class ShaderResource;
class SceneComponentScreenQuad;
class SceneComponentCameraRay;
class SceneComponentBackground;
class UnorderedAccess;
class GeometryGeneric;
class Timer;

class ApplicationMultiLineCompute : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationMultiLineCompute(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationMultiLineCompute();

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

    struct DrawResources
    {
        std::shared_ptr<SceneComponentScreenQuad> _screen_quad;
        std::shared_ptr<SceneComponentCameraRay> _camera_ray;
        std::shared_ptr<SceneComponentBackground> _background;

        //std::shared_ptr<UnorderedAccess> _multi_line_data_pos_thick;
        //std::shared_ptr<UnorderedAccess> _multi_line_data_dir_length;
        //std::shared_ptr<UnorderedAccess> _multi_line_data_colour;

        //std::shared_ptr<Shader> _multi_line_compute;
        //std::shared_ptr<Shader> _multi_line_shader;
        //std::shared_ptr<GeometryGeneric> _multi_line_geometry;

    };
    std::unique_ptr<DrawResources> _draw_resources;

    std::unique_ptr<Timer> _timer;
    float _timer_accumulate_wrapped;

};
