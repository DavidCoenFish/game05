#include "application_character_00_pch.h"
#include "scene_component/scene_component_background.h"

#include "scene_component/scene_component_camera_ray.h"

#include "common/file_system/file_system.h"
#include "common/math/angle.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/shader_constant_buffer.h"

struct ConstantBufferSky
{
    VectorFloat2 _sun_azimuth_altitude;
    VectorFloat2 _sun_range;

    VectorFloat3 _sun_tint;
    float _sky_spread;

    VectorFloat3 _sky_tint;
    float _sky_turbitity;

    VectorFloat3 _ground_tint;
    float _pad0;

    VectorFloat3 _fog_tint;
    float _pad1;

};

std::shared_ptr<SceneComponentBackground> SceneComponentBackground::Factory(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
    const std::filesystem::path& in_root_path,
    const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture
    )
{
    std::shared_ptr<Shader> shader_background;
    std::shared_ptr<ShaderConstantBuffer> shader_background_constant_buffer;
    std::shared_ptr<Shader> shader_grid;
    std::shared_ptr<ShaderConstantBuffer> shader_grid_constant_buffer;



    auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_vertex.cso");

    // Shader background
    {
        auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_pixel_background.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        ShaderPipelineStateData shader_pipeline_state_data(
            in_input_element_desc_array,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN,
            render_target_format,
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
        );

        std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
        array_shader_resource_info.push_back(ShaderResourceInfo::FactoryDataSampler(
            in_camera_ray_texture,
            D3D12_SHADER_VISIBILITY_PIXEL
            ));

        std::vector<std::shared_ptr<ConstantBufferInfo>> array_shader_constants_info;
        array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
            ConstantBufferSky(),
            D3D12_SHADER_VISIBILITY_PIXEL
            ));

        shader_background = in_draw_system->MakeShader(
            in_command_list,
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info,
            array_shader_constants_info
        );

        shader_background_constant_buffer = shader_background->MakeShaderConstantBuffer(in_draw_system);
    }

    return std::make_shared<SceneComponentBackground>(
        shader_background,
        shader_background_constant_buffer
        );
}

SceneComponentBackground::SceneComponentBackground(
    const std::shared_ptr<Shader>& in_shader_background,
    const std::shared_ptr<ShaderConstantBuffer>& in_shader_background_constant_buffer
    )
    : _shader_background(in_shader_background)
    , _shader_background_constant_buffer(in_shader_background_constant_buffer)
{
    // Nop
}

SceneComponentBackground::~SceneComponentBackground()
{
    // Nop
}

void SceneComponentBackground::Draw(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    const std::shared_ptr<GeometryGeneric>& in_screen_quad,
    const CameraConstantBufferB0& in_camera_constant_buffer
    )
{
    in_draw_system_frame->SetRenderTarget(
        in_draw_system->GetRenderTargetBackBuffer()
        );

    // Draw background
#if 1
    if ((nullptr != _shader_background) && (nullptr != _shader_background_constant_buffer))
    {
        auto& buffer_background_sky = _shader_background_constant_buffer->GetConstant<ConstantBufferSky>(0);

        buffer_background_sky._sun_azimuth_altitude = VectorFloat2(Angle::DegToRadian(45.0f), Angle::DegToRadian(45.0f));
        buffer_background_sky._sun_range = VectorFloat2(0.05f, 0.2f);//1.0f, 5.0f);
        buffer_background_sky._sun_tint = VectorFloat3(255.0f / 255.0f, 245.0f / 255.0f, 235.0f / 255.0f);
        buffer_background_sky._sky_spread = -0.75f; //-0.5
        buffer_background_sky._sky_tint = VectorFloat3(10.0f / 255.0f, 10.0f / 255.0f, 255.0f / 255.0f);
        buffer_background_sky._sky_turbitity = 25.0f; //5.0f; //10.0f;
        buffer_background_sky._ground_tint = VectorFloat3(32.0f / 255.0f, 16.0f / 255.0f, 2.0f / 255.0f);
        buffer_background_sky._fog_tint = VectorFloat3(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f);

        in_draw_system_frame->SetShader(_shader_background, _shader_background_constant_buffer);
        in_draw_system_frame->Draw(in_screen_quad);
    }
#endif
}
