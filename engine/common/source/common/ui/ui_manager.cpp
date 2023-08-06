#include "common/common_pch.h"
#include "common/ui/ui_manager.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/heap_wrapper/heap_wrapper.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/file_system/file_system.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/ui/ui_geometry.h"
#include "common/util/vector_helper.h"

namespace
{
    static const std::vector<D3D12_INPUT_ELEMENT_DESC> s_input_element_desc_array({
        D3D12_INPUT_ELEMENT_DESC
        {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        D3D12_INPUT_ELEMENT_DESC
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        }
    });
}

class UIManagerImplementation
{
public:
    UIManagerImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        )
    {
        auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "ui_block_vertex.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "ui_block_pixel.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        ShaderPipelineStateData shader_pipeline_state_data(
            s_input_element_desc_array,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN,
            // DXGI_FORMAT_D32_FLOAT,
            render_target_format,
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
            );

        std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
        array_shader_resource_info.push_back(
            ShaderResourceInfo::FactorySampler(nullptr, D3D12_SHADER_VISIBILITY_PIXEL)
            );

        _shader = in_draw_system->MakeShader(
            in_command_list,
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info
            );
    }

    void DrawHelper(
        DrawSystemFrame* const in_frame,
        IRenderTarget* const in_render_target,
        GeometryGeneric* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_texture_handle
        )
    {
        in_frame->SetRenderTarget(in_render_target);
        _shader->SetShaderResourceViewHandle(0, in_texture_handle);
        in_frame->Draw(in_geometry);
        return;
    }

    void DrawHierarchy(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        UIHierarchyNode* const in_node,
        const float in_ui_scale
        )
    {
        in_draw_system;
        in_frame;
        in_node;
        in_ui_scale;
        return;
    }

private:
    std::shared_ptr<Shader> _shader;

};

const std::vector<D3D12_INPUT_ELEMENT_DESC>& UIManager::GetInputElementDesc()
{
    return s_input_element_desc_array;
}

UIManager::UIManager(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::filesystem::path& in_root_path
    )
{
    _implementation = std::make_unique<UIManagerImplementation>(
        in_draw_system, 
        in_command_list,
        in_root_path
        );
}

UIManager::~UIManager()
{
    // Nop
}

std::shared_ptr<GeometryGeneric> UIManager::GeometryHelper(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
{
    return UIGeometry::GeometryHelper(
        in_draw_system,
        in_command_list,
        in_pos,
        in_uv
        );
}

void UIManager::DrawHelper(
    DrawSystemFrame* const in_frame,
    IRenderTarget* const in_render_target,
    GeometryGeneric* const in_geometry,
    const std::shared_ptr<HeapWrapperItem>& in_texture_handle
    )
{
    _implementation->DrawHelper(
        in_frame,
        in_render_target,
        in_geometry,
        in_texture_handle
        );
    return;
}

void UIManager::DrawHierarchy(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    UIHierarchyNode* const in_node,
    const float in_ui_scale
    )
{
    _implementation->DrawHierarchy(
        in_draw_system,
        in_frame,
        in_node,
        in_ui_scale
        );
    return;
}
