#pragma once
#include "common/math/vector_float3.h"
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

    float _fov_vertical;
    float _fov_horizontal_calculated;

    VectorFloat3 _camera_pos;
    VectorFloat3 _camera_at;
    VectorFloat3 _camera_up;
    float _camera_far;

    VectorFloat3 _line_pos;
    float _radian_per_pixel;
    VectorFloat3 _line_at;
    float _line_length;

};
