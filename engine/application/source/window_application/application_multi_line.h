#pragma once
#include "common/math/vector_float3.h"
#include "common/window/i_window_application.h"

#define DSC_UNORDED_ACCESS 1

class DrawSystem;
class Shader;
class ShaderConstantBuffer;
class ShaderResource;
class UnorderedAccess;
class GeometryGeneric;
class Timer;

class ApplicationMultiLine : public IWindowApplication
{
public:
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    ApplicationMultiLine(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationMultiLine();

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
        std::shared_ptr<Shader> _background_shader;
        std::shared_ptr<ShaderConstantBuffer> _background_shader_constant_buffer;
        std::shared_ptr<Shader> _background_shader_grid;
        std::shared_ptr<ShaderConstantBuffer> _background_shader_grid_constant_buffer;
        std::shared_ptr<GeometryGeneric> _background_geometry;

        std::shared_ptr<Shader> _multi_line_compute;

#if defined(DSC_UNORDED_ACCESS)
        std::shared_ptr<UnorderedAccess> _multi_line_data_pos_thick;
        std::shared_ptr<UnorderedAccess> _multi_line_data_dir_length;
        std::shared_ptr<UnorderedAccess> _multi_line_data_colour;
#else
        std::shared_ptr<ShaderResource> _multi_line_data_pos_thick;
        std::shared_ptr<ShaderResource> _multi_line_data_dir_length;
        std::shared_ptr<ShaderResource> _multi_line_data_colour;
#endif

        std::shared_ptr<Shader> _multi_line_shader;
        std::shared_ptr<ShaderConstantBuffer> _multi_line_shader_constant_buffer;
        std::shared_ptr<GeometryGeneric> _multi_line_geometry;

    };
    std::unique_ptr<DrawResources> _draw_resources;

    std::unique_ptr<Timer> _timer;

    float _fov_vertical;
    float _fov_horizontal_calculated;
    float _radian_per_pixel;
    float _unit_pixel_size;

    VectorFloat3 _camera_pos;
    VectorFloat3 _camera_at;
    VectorFloat3 _camera_up;
    float _camera_far;

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
