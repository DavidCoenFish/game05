#include "common/common_pch.h"
#include "common/bezier/bezier_manager.h"

#include "common/bezier/bezier_curve.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/file_system/file_system.h"

namespace
{
    static const std::vector<D3D12_INPUT_ELEMENT_DESC> s_input_element_desc_array({
        D3D12_INPUT_ELEMENT_DESC
        {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        //p0 the start
        D3D12_INPUT_ELEMENT_DESC
        {
            "COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        //p1 a combined gradient target for both p0 and p1
        D3D12_INPUT_ELEMENT_DESC
        {
            "COLOR", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        //p2 the end
        D3D12_INPUT_ELEMENT_DESC
        {
            "COLOR", 2, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        // thickness at p0, p1
        D3D12_INPUT_ELEMENT_DESC
        {
            "COLOR", 3, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        // dim, the pixel width/height that the quad is drawing to
        D3D12_INPUT_ELEMENT_DESC
        {
            "COLOR", 4, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        // uv, the uv in the quad to work out the sample point. position can be rahter arbitary so can not base uv on pos
        D3D12_INPUT_ELEMENT_DESC
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        }
    });


}

class BezierManagerImplementation
{
public:
    BezierManagerImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        )
    {
        {
            auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "bezier_vertex.cso");
            auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "bezier_pixel.cso");
            std::vector<DXGI_FORMAT> render_target_format;
            render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
            const auto blend_desc = ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied();
            ShaderPipelineStateData shader_pipeline_state_data(
                s_input_element_desc_array,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                DXGI_FORMAT_UNKNOWN,
                render_target_format,
                blend_desc,
                CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                CD3DX12_DEPTH_STENCIL_DESC()
                );

            _shader = in_draw_system->MakeShader(
                in_command_list,
                shader_pipeline_state_data,
                vertex_shader_data,
                nullptr,
                pixel_shader_data
                );
        }

    }

    ~BezierManagerImplementation()
    {
        // nop
    }

    void DrawBezierCurve(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        BezierCurve* const in_bezier_curve
        )
    {
        in_bezier_curve->Draw(
            in_draw_system,
            in_draw_system_frame,
            _shader
            );
    }

private:
    std::shared_ptr<Shader> _shader;

};

const std::vector<D3D12_INPUT_ELEMENT_DESC>& BezierManager::GetInputElementDesc()
{
    return s_input_element_desc_array;
}

BezierManager::BezierManager(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::filesystem::path& in_root_path
    )
{
    _implementation = std::make_unique<BezierManagerImplementation>(
        in_draw_system,
        in_command_list,
        in_root_path
        );
}

BezierManager::~BezierManager()
{
    // Nop
}

void BezierManager::DrawBezierCurve(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    BezierCurve* const in_bezier_curve
    )
{
    _implementation->DrawBezierCurve(
        in_draw_system,
        in_draw_system_frame,
        in_bezier_curve
        );
}

