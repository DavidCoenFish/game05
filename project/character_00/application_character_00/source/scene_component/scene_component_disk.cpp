#include "application_character_00_pch.h"

#include "scene_component/scene_component_disk.h"
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

struct ConstantBufferDisk
{
    VectorFloat3 _disk_xy_r;
    float _pad0;

    VectorFloat4 _colour;
};

std::shared_ptr<SceneComponentDisk> SceneComponentDisk::Factory(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
    const std::filesystem::path& in_root_path,
    const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture,
	const VectorFloat4& in_colour,
	const VectorFloat3& in_disk_xy_radius
    )
{
    std::shared_ptr<Shader> shader_disk;
    std::shared_ptr<ShaderConstantBuffer> shader_disk_constant_buffer;

    auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_vertex.cso");

    {
        auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_pixel_disk.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        const auto blend_desc = ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied();

        ShaderPipelineStateData shader_pipeline_state_data(
            in_input_element_desc_array,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN,
            render_target_format,
            blend_desc,
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC()
        );

        std::vector < std::shared_ptr < ShaderResourceInfo > > array_shader_resource_info;
        array_shader_resource_info.push_back(ShaderResourceInfo::FactoryDataSampler(
            in_camera_ray_texture,
            D3D12_SHADER_VISIBILITY_PIXEL
        ));

        std::vector < std::shared_ptr < ConstantBufferInfo > > array_shader_constants_info;
        array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
            CameraConstantBufferB0(),
            D3D12_SHADER_VISIBILITY_PIXEL
        ));
        array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
            ConstantBufferDisk(),
            D3D12_SHADER_VISIBILITY_PIXEL
        ));

        shader_disk = in_draw_system->MakeShader(
            in_command_list,
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info,
            array_shader_constants_info
        );

        shader_disk_constant_buffer = shader_disk->MakeShaderConstantBuffer(in_draw_system);
    }

    return std::make_shared<SceneComponentDisk>(
        shader_disk,
        shader_disk_constant_buffer
        );
}

SceneComponentDisk::SceneComponentDisk(
    const std::shared_ptr<Shader>& in_shader_disk,
    const std::shared_ptr<ShaderConstantBuffer>& in_shader_disk_constant_buffer
    )
    : _shader_disk(in_shader_disk)
    , _shader_disk_constant_buffer(in_shader_disk_constant_buffer)
{
    // Nop
}

SceneComponentDisk::~SceneComponentDisk()
{
    // Nop
}

void SceneComponentDisk::Draw(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    const std::shared_ptr<GeometryGeneric>& in_screen_quad,
    const CameraConstantBufferB0& in_camera_constant_buffer
    )
{
    in_draw_system_frame->SetRenderTarget(
        in_draw_system->GetRenderTargetBackBuffer()
        );

    if ((nullptr != _shader_disk) && (nullptr != _shader_disk_constant_buffer))
    {
        _shader_disk_constant_buffer->GetConstant<CameraConstantBufferB0>(0) = in_camera_constant_buffer;

        in_draw_system_frame->SetShader(_shader_disk, _shader_disk_constant_buffer);
        in_draw_system_frame->Draw(in_screen_quad);
    }
}

void SceneComponentDisk::SetDisk(
	const VectorFloat3& in_disk_xy_radius
	)
{
    if ((nullptr != _shader_disk_constant_buffer))
    {
        auto& buffer = _shader_disk_constant_buffer->GetConstant<ConstantBufferDisk>(1);
		buffer._disk_xy_r = in_disk_xy_radius;
	}
}
