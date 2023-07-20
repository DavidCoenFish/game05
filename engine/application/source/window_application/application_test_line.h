#pragma once
#include "common/math/vector_float3.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class Shader;
class GeometryGeneric;
class Timer;

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
    virtual void OnKey(
        const int in_vk_code, 
        const int in_scan_code, 
        const bool in_repeat_flag, 
        const int in_repeat_count, 
        bool in_up_flag
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr < DrawSystem > _draw_system;
    std::shared_ptr < Shader > _shader;
    std::shared_ptr < GeometryGeneric > _geometry;

    std::shared_ptr < Shader > _shader_background;

    float _fov_vertical;
    float _fov_horizontal_calculated;
    float _unit_pixel_size;

    VectorFloat3 _camera_pos;
    VectorFloat3 _camera_at;
    VectorFloat3 _camera_up;
    float _camera_far;

    std::unique_ptr < Timer > _timer;

    // Camera Pos
    bool _input_q;
    bool _input_w;
    bool _input_e;
    bool _input_a;
    bool _input_s;
    bool _input_d;

    // Camera Rot
    bool _input_u;
    bool _input_i;
    bool _input_o;
    bool _input_j;
    bool _input_k;
    bool _input_l;

};
