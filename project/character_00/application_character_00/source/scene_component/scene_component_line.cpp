#include "application_character_00_pch.h"
#include "scene_component/scene_component_line.h"

#include "common/file_system/file_system.h"
#include "common/math/angle.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/math/vector_float4.h"
#include "common/math/dsc_math.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "scene_component/scene_component_camera_ray.h"

struct ConstantBufferLine
{
    VectorFloat4 _ray_origin_thickness;
    VectorFloat4 _ray_normal_length;
    VectorFloat4 _ray_color;
};

namespace
{
	const VectorFloat4 CalculateNormalLength(const VectorFloat3& in_point_a, const VectorFloat3& in_point_b)
	{
		const VectorFloat3 offset = in_point_b - in_point_a;
		const float length = DscMath::Length(offset);
		const VectorFloat3 normal = DscMath::Normal(offset, length);
		return VectorFloat4(
			normal.GetX(),
			normal.GetY(),
			normal.GetZ(),
			length
			);
	}
}

std::shared_ptr<SceneComponentLine> SceneComponentLine::Factory(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
    const std::filesystem::path& in_root_path,
    const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture,
	const VectorFloat3& in_point_a,
	const VectorFloat3& in_point_b,
	const VectorFloat4& in_colour,
	const float in_line_thickness
    )
{
    std::shared_ptr<Shader> shader_line;
    std::shared_ptr<ShaderConstantBuffer> shader_line_constant_buffer;

    auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_vertex.cso");

    {
        auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "screen_quad_pixel_line.cso");
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
		const VectorFloat4 ray_origin_thickness(
			in_point_a.GetX(),
			in_point_a.GetY(),
			in_point_a.GetZ(),
			in_line_thickness
			);
		const VectorFloat4 ray_normal_length = CalculateNormalLength(in_point_a, in_point_b);

        array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
            ConstantBufferLine(
				{
					ray_origin_thickness, 
					ray_normal_length,
					in_colour
				}),
            D3D12_SHADER_VISIBILITY_PIXEL
        ));


        shader_line = in_draw_system->MakeShader(
            in_command_list,
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info,
            array_shader_constants_info
        );

        shader_line_constant_buffer = shader_line->MakeShaderConstantBuffer(in_draw_system);
    }

    return std::make_shared<SceneComponentLine>(
        shader_line,
        shader_line_constant_buffer
        );
}

SceneComponentLine::SceneComponentLine(
    const std::shared_ptr<Shader>& in_shader_line,
    const std::shared_ptr<ShaderConstantBuffer>& in_shader_line_constant_buffer
    )
    : _shader_line(in_shader_line)
    , _shader_line_constant_buffer(in_shader_line_constant_buffer)
{
    // Nop
}

SceneComponentLine::~SceneComponentLine()
{
    // Nop
}

void SceneComponentLine::Draw(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    const std::shared_ptr<GeometryGeneric>& in_screen_quad,
    const CameraConstantBufferB0& in_camera_constant_buffer
    )
{
    in_draw_system_frame->SetRenderTarget(
        in_draw_system->GetRenderTargetBackBuffer()
        );

    if ((nullptr != _shader_line) && (nullptr != _shader_line_constant_buffer))
    {
        _shader_line_constant_buffer->GetConstant<CameraConstantBufferB0>(0) = in_camera_constant_buffer;

        in_draw_system_frame->SetShader(_shader_line, _shader_line_constant_buffer);
        in_draw_system_frame->Draw(in_screen_quad);
    }
}

void SceneComponentLine::SetLine(
	const VectorFloat3& in_point_a,
	const VectorFloat3& in_point_b
	)
{
    if ((nullptr != _shader_line_constant_buffer))
    {
        auto& buffer = _shader_line_constant_buffer->GetConstant<ConstantBufferLine>(1);
		buffer._ray_normal_length = CalculateNormalLength(in_point_a, in_point_b);
	}
}
