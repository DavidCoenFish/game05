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

struct ConstantBufferLineDisk
{
    VectorFloat4 _circle_pos_radius;
    VectorFloat4 _circle_normal_thickness;
    VectorFloat4 _color;
};

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
    in_draw_system_frame->SetRenderTarget(
        in_draw_system->GetRenderTargetBackBuffer()
        );

    if ((nullptr != _shader) && (nullptr != _shader_constant_buffer))
    {
        _shader_constant_buffer->GetConstant<CameraConstantBufferB0>(0) = in_camera_constant_buffer;

        in_draw_system_frame->SetShader(_shader, _shader_constant_buffer);
        in_draw_system_frame->Draw(in_screen_quad);
    }
}
