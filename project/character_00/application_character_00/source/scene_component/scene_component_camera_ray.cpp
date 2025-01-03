#include "application_character_00_pch.h"
#include "scene_component/scene_component_camera_ray.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/draw_system/shader/constant_buffer_info.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/math/quaternion_float.h"
#include "common/math/matrix_float33.h"
#include "common/window/window_application_param.h"

namespace
{
    static const VectorFloat3 s_camera_pos(-1.0f, 0.0f, 0.0f);
    static const VectorFloat3 s_camera_at(1.0f, 0.0f, 0.0f);
    static const VectorFloat3 s_camera_up(0.0f, 1.0f, 0.0f);
    static const float s_camera_far = 1000.0f;
}

std::shared_ptr<SceneComponentCameraRay> SceneComponentCameraRay::Factory(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
    const std::filesystem::path& in_root_path,
    const VectorFloat3& in_initial_camera_pos 
    )
{
    const std::vector<RenderTargetFormatData> target_format_data_array({
        RenderTargetFormatData(DXGI_FORMAT_R32G32B32A32_FLOAT)
    });

    auto render_target_texture = in_draw_system->MakeRenderTargetTexture(
        in_command_list,
        target_format_data_array,
        RenderTargetDepthData(),
        in_draw_system->GetRenderTargetBackBuffer()->GetSize(),
        true
        );

    auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_vertex.cso");
    auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_pixel_camera_ray.cso");
    std::vector < DXGI_FORMAT > render_target_format;
    render_target_format.push_back(DXGI_FORMAT_R32G32B32A32_FLOAT);
    ShaderPipelineStateData shader_pipeline_state_data(
        in_input_element_desc_array,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        DXGI_FORMAT_UNKNOWN,
        // DXGI_FORMAT_D32_FLOAT,
        render_target_format,
        CD3DX12_BLEND_DESC(D3D12_DEFAULT),
        CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
        CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
    );

    std::vector < std::shared_ptr < ShaderResourceInfo > > array_shader_resource_info;
    std::vector < std::shared_ptr < ConstantBufferInfo > > array_shader_constants_info;

    array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
        CameraConstantBufferB0(),
        D3D12_SHADER_VISIBILITY_PIXEL
        ));

    auto shader_camera_ray = in_draw_system->MakeShader(
        in_command_list,
        shader_pipeline_state_data,
        vertex_shader_data,
        nullptr,
        pixel_shader_data,
        array_shader_resource_info,
        array_shader_constants_info
        );

    auto shader_camera_ray_constant_buffer = shader_camera_ray->MakeShaderConstantBuffer(in_draw_system);

    return std::make_shared<SceneComponentCameraRay>(
        shader_camera_ray,
        shader_camera_ray_constant_buffer,
        render_target_texture,
		in_initial_camera_pos
        );
}

SceneComponentCameraRay::SceneComponentCameraRay(
    const std::shared_ptr<Shader>& in_shader_camera_ray,
    const std::shared_ptr<ShaderConstantBuffer>& in_shader_camera_ray_constant_buffer,
    const std::shared_ptr<RenderTargetTexture>& in_render_target_texture,
	const VectorFloat3& in_initial_camera_pos
    )
    : _constant_buffer({
        in_initial_camera_pos,
        0.0f, // fov horizontal
        s_camera_at,
        Angle::DegToRadian(145.0f), // fov vertical
        s_camera_up,
        1000.0f // camera far
    })
    , _change_since_last_update(true)
    , _shader_camera_ray(in_shader_camera_ray)
    , _shader_camera_ray_constant_buffer(in_shader_camera_ray_constant_buffer)
    , _render_target_texture(in_render_target_texture)
    , _input_q(false)
    , _input_w(false)
    , _input_e(false)
    , _input_a(false)
    , _input_s(false)
    , _input_d(false)
    , _input_u(false)
    , _input_i(false)
    , _input_o(false)
    , _input_j(false)
    , _input_k(false)
    , _input_l(false)
{
    // Nop
}

SceneComponentCameraRay::~SceneComponentCameraRay()
{
    // Nop
}

void SceneComponentCameraRay::Update(
    const float in_time_delta_seconds,
    DrawSystemFrame* const in_draw_system_frame,
    const std::shared_ptr<GeometryGeneric>& in_screen_quad
    )
{
    // camera pos update
    {
        const auto camera_right = Cross(_constant_buffer._camera_at, _constant_buffer._camera_up);

        if (true == _input_q)
        {
            _constant_buffer._camera_pos -= (_constant_buffer._camera_up * in_time_delta_seconds);
            _change_since_last_update = true;
        }
        if (true == _input_w)
        {
            _constant_buffer._camera_pos += (_constant_buffer._camera_at * in_time_delta_seconds);
            _change_since_last_update = true;
        }
        if (true == _input_e)
        {
            _constant_buffer._camera_pos += (_constant_buffer._camera_up * in_time_delta_seconds);
            _change_since_last_update = true;
        }
        if (true == _input_a)
        {
            _constant_buffer._camera_pos -= (camera_right * in_time_delta_seconds);
            _change_since_last_update = true;
        }
        if (true == _input_s)
        {
            _constant_buffer._camera_pos -= (_constant_buffer._camera_at * in_time_delta_seconds);
            _change_since_last_update = true;
        }
        if (true == _input_d)
        {
            _constant_buffer._camera_pos += (camera_right * in_time_delta_seconds);
            _change_since_last_update = true;
        }

        bool rotation_flag = false;
        auto rotation = QuaternionFloat::FactoryIdentity();
        if (true == _input_u)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_constant_buffer._camera_at, in_time_delta_seconds);
        }
        if (true == _input_i)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(camera_right, -in_time_delta_seconds);
        }
        if (true == _input_o)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_constant_buffer._camera_at, -in_time_delta_seconds);
        }
        if (true == _input_j)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_constant_buffer._camera_up, -in_time_delta_seconds);
        }
        if (true == _input_k)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(camera_right, in_time_delta_seconds);
        }
        if (true == _input_l)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_constant_buffer._camera_up, in_time_delta_seconds);
        }

        if (true == rotation_flag)
        {
            const auto matrix = MatrixFloat33::FactoryQuaternionFloat(rotation);
            //matrix.in
            _constant_buffer._camera_at = matrix * _constant_buffer._camera_at;
            _constant_buffer._camera_up = matrix * _constant_buffer._camera_up;

            // Normalise
            _constant_buffer._camera_at.NormaliseSelf();
            const auto temp = Cross(_constant_buffer._camera_at, _constant_buffer._camera_up);
            _constant_buffer._camera_up = Cross(temp, _constant_buffer._camera_at);
            _constant_buffer._camera_up.NormaliseSelf();

            _change_since_last_update = true;
        }
    }

    // Draw if needed
    if (true == _change_since_last_update)
    {
        _change_since_last_update = false;

        in_draw_system_frame->SetRenderTarget(
            _render_target_texture.get()
            );

        if ((nullptr != _shader_camera_ray) && (nullptr != _shader_camera_ray_constant_buffer))
        {
            _shader_camera_ray_constant_buffer->GetConstant<CameraConstantBufferB0>(0) = _constant_buffer;

            in_draw_system_frame->SetShader(_shader_camera_ray, _shader_camera_ray_constant_buffer);
            in_draw_system_frame->Draw(in_screen_quad);
        }

        in_draw_system_frame->SetRenderTarget(
            nullptr
            );
    }

    //LOG_CONSOLE("SceneComponentCameraRay::Update _constant_buffer._camera_pos[%f, %f, %f]", _constant_buffer._camera_pos[0], _constant_buffer._camera_pos[1], _constant_buffer._camera_pos[2]);
}

void SceneComponentCameraRay::OnWindowSizeChanged(
    const int in_width,
    const int in_height
    )
{
    const float aspect = (float)in_width / (float)in_height;
    _constant_buffer._fov_horizontal = aspect * _constant_buffer._fov_vertical;
    _constant_buffer._radian_per_pixel = _constant_buffer._fov_vertical / (float)in_height;
    _constant_buffer._unit_pixel_size = sin(_constant_buffer._radian_per_pixel);
    _change_since_last_update = true;
    return;
}

void SceneComponentCameraRay::OnKey(
    const int in_vk_code,
    bool in_up_flag
    )
{
    if ('Q' == in_vk_code)
    {
        _input_q = (false == in_up_flag);
    }
    else if ('W' == in_vk_code)
    {
        _input_w = (false == in_up_flag);
    }
    else if ('E' == in_vk_code)
    {
        _input_e = (false == in_up_flag);
    }
    else if ('A' == in_vk_code)
    {
        _input_a = (false == in_up_flag);
    }
    else if ('S' == in_vk_code)
    {
        _input_s = (false == in_up_flag);
    }
    else if ('D' == in_vk_code)
    {
        _input_d = (false == in_up_flag);
    }

    else if ('U' == in_vk_code)
    {
        _input_u = (false == in_up_flag);
    }
    else if ('I' == in_vk_code)
    {
        _input_i = (false == in_up_flag);
    }
    else if ('O' == in_vk_code)
    {
        _input_o = (false == in_up_flag);
    }
    else if ('J' == in_vk_code)
    {
        _input_j = (false == in_up_flag);
    }
    else if ('K' == in_vk_code)
    {
        _input_k = (false == in_up_flag);
    }
    else if ('L' == in_vk_code)
    {
        _input_l = (false == in_up_flag);
    }
    else if ((VK_HOME == in_vk_code) && (false == in_up_flag))
    {
        _constant_buffer._camera_pos = s_camera_pos;
        _constant_buffer._camera_at = s_camera_at;
        _constant_buffer._camera_up = s_camera_up;
        _change_since_last_update = true;
    }

    return;
}

const std::shared_ptr<HeapWrapperItem> SceneComponentCameraRay::GetHeapWrapperCameraRay() const
{
    return _render_target_texture->GetShaderResourceHeapWrapperItem(0);
}
