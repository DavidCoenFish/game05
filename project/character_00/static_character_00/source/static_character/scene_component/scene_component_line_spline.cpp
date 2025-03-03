#include "static_character/static_character_pch.h"
#include "static_character/scene_component/scene_component_line_spline.h"

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
#include "static_character/scene_component/scene_component_camera_ray.h"

struct ConstantBufferLineSpline
{
    VectorFloat4 _p0_thickness;
    VectorFloat4 _p1;
    VectorFloat4 _p2;
    VectorFloat4 _color;
};

std::shared_ptr<SceneComponentLineSpline> SceneComponentLineSpline::Factory(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
    const std::filesystem::path& in_root_path,
    const std::shared_ptr<HeapWrapperItem>& in_camera_ray_texture,
    const VectorFloat3& in_p0,
    const VectorFloat3& in_p1,
    const VectorFloat3& in_p2,
    const VectorFloat4& in_colour,
    const float in_thickness
)
{
    std::shared_ptr<Shader> shader_line;
    std::shared_ptr<ShaderConstantBuffer> shader_line_constant_buffer;

    auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "static_character_00" / "screen_quad_vertex.cso");

    {
        auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "static_character_00" / "screen_quad_pixel_line_spline.cso");
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
            ConstantBufferLineSpline(
                {
                    VectorFloat4(in_p0.GetX(), in_p0.GetY(), in_p0.GetZ(), in_thickness),
                    VectorFloat4(in_p1.GetX(), in_p1.GetY(), in_p1.GetZ()),
                    VectorFloat4(in_p2.GetX(), in_p2.GetY(), in_p2.GetZ()),
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

    return std::make_shared<SceneComponentLineSpline>(
        shader_line,
        shader_line_constant_buffer
        );
}


SceneComponentLineSpline::SceneComponentLineSpline(
    const std::shared_ptr<Shader>& in_shader,
    const std::shared_ptr<ShaderConstantBuffer>& in_shader_constant_buffer
    )
    : _shader(in_shader)
    , _shader_constant_buffer(in_shader_constant_buffer)
{
    // Nop
}

SceneComponentLineSpline::~SceneComponentLineSpline()
{
    // Nop
}

void SceneComponentLineSpline::Draw(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    const std::shared_ptr<GeometryGeneric>& in_screen_quad,
    const CameraConstantBufferB0& in_camera_constant_buffer
    )
{
    if ((nullptr != _shader) && (nullptr != _shader_constant_buffer))
    {
        _shader_constant_buffer->GetConstant<CameraConstantBufferB0>(0) = in_camera_constant_buffer;

        in_draw_system_frame->SetShader(_shader, _shader_constant_buffer);
        in_draw_system_frame->Draw(in_screen_quad);
    }
}

void SceneComponentLineSpline::SetData(
    const VectorFloat3& in_p0,
    const VectorFloat3& in_p1,
    const VectorFloat3& in_p2
    )
{
    if ((nullptr != _shader_constant_buffer))
    {
        auto& buffer = _shader_constant_buffer->GetConstant<ConstantBufferLineSpline>(1);
        buffer._p0_thickness.Set(in_p0.GetX(), in_p0.GetY(), in_p0.GetZ(), buffer._p0_thickness.GetZ());
        buffer._p1.Set(in_p1.GetX(), in_p1.GetY(), in_p1.GetZ(), 0.0f);
        buffer._p2.Set(in_p2.GetX(), in_p2.GetY(), in_p2.GetZ(), 0.0f);
    }
}


