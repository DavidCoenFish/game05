#pragma once
#include "common/math/vector_float3.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class Shader;
class ShaderConstantBuffer;
class ShaderResource;
class GeometryGeneric;
class RenderTargetTexture;

struct CameraConstantBufferB0
{
    VectorFloat3 _camera_pos;
    float _fov_horizontal;

    VectorFloat3 _camera_at;
    float _fov_vertical;

    VectorFloat3 _camera_up;
    float _far;

    float _unit_pixel_size; //sin(radian_per_pixel)
    float _radian_per_pixel;
    float _pad0[2];
};


/*
    move debug camera input into a different component?
*/
class SceneComponentCameraRay
{
public:
    static std::shared_ptr<SceneComponentCameraRay> Factory(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
        const std::filesystem::path& in_root_path
        );

    SceneComponentCameraRay(
        const std::shared_ptr<Shader>& in_shader_camera_ray,
        const std::shared_ptr<ShaderConstantBuffer>& in_shader_camera_ray_constant_buffer,
        const std::shared_ptr<RenderTargetTexture>& in_render_target_texture
        );

    ~SceneComponentCameraRay();

    void Update(
        const float in_time_delta_seconds,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad
        );

    void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        );

    void OnKey(
        const int in_vk_code,
        bool in_up_flag
        );

    const CameraConstantBufferB0& GetConstantBufferB0() const
    {
        return _constant_buffer;
    }

    const std::shared_ptr<HeapWrapperItem> GetHeapWrapperCameraRay() const;

private:
    CameraConstantBufferB0 _constant_buffer;
    bool _change_since_last_update;

    std::shared_ptr<Shader> _shader_camera_ray;
    std::shared_ptr<ShaderConstantBuffer> _shader_camera_ray_constant_buffer;
    std::shared_ptr<RenderTargetTexture> _render_target_texture;

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
