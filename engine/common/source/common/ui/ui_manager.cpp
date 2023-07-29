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
#include "common/util/vector_helper.h"

namespace
{
    struct UIConstantBufferB0
    {
        VectorInt4 _block_pos; // Low xy, high xy 
        VectorFloat4 _block_uv; // Low xy, high xy [0 ... 1]
        VectorInt2 _target_size; // width, height of the thing we are drawing into
    };
}

class UiManagerImplementation
{
public:
    UiManagerImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_shader_root_path
        )
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> input_element_desc_array(
            {
                D3D12_INPUT_ELEMENT_DESC
                {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32_FLOAT,
                    0,
                    D3D12_APPEND_ALIGNED_ELEMENT,
                    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                    0 // UINT InstanceDataStepRate;
                }
            }
        );

        const auto vertex_data = VectorHelper::FactoryArrayLiteral(
            {
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,
            }
        );

        _geometry_generic = in_draw_system->MakeGeometryGeneric(
            in_command_list,
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            input_element_desc_array,
            vertex_data,
            2
            );

        auto vertex_shader_data = FileSystem::SyncReadFile(in_shader_root_path / "ui_block_vertex.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_shader_root_path / "ui_block_pixel.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_R32G32B32A32_FLOAT);
        ShaderPipelineStateData shader_pipeline_state_data(
            input_element_desc_array,
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
            UIConstantBufferB0(),
            D3D12_SHADER_VISIBILITY_PIXEL
            ));

        _shader = in_draw_system->MakeShader(
            in_command_list,
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info,
            array_shader_constants_info
            );
    }

    void DrawBlock(
        const std::unique_ptr<DrawSystemFrame>& in_frame,
        const VectorInt2& in_target_size,
        const VectorInt4& in_block_size,
        const VectorFloat4& in_block_uv,
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        )
    {
        auto shader = _shader.get();
        if (shader)
        {
            shader->SetShaderResourceViewHandle(0, in_shader_resource_view_handle);
            auto& buffer = _shader->GetConstant<UIConstantBufferB0>(0);
            buffer._block_pos = in_block_size;
            buffer._block_uv = in_block_uv;
            //buffer._block_uv.operator=(in_block_uv);
            buffer._target_size = in_target_size;

            in_frame->SetShader(_shader.get());
            in_frame->Draw(_geometry_generic.get());
        }
    }

private:
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<GeometryGeneric> _geometry_generic;

};

UiManager::UiManager(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::filesystem::path& in_shader_root_path
    )
{
    _implementation = std::make_unique<UiManagerImplementation>(
        in_draw_system, 
        in_command_list,
        in_shader_root_path
        );
}

void UiManager::DrawBlock(
    const std::unique_ptr<DrawSystemFrame>& in_frame,
    const VectorInt2& in_target_size,
    const VectorInt4& in_block_size,
    const VectorFloat4& in_block_uv,
    const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
    )
{
    _implementation->DrawBlock(
        in_frame,
        in_target_size,
        in_block_size,
        in_block_uv,
        in_shader_resource_view_handle
        );
    return;
}
