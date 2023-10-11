#include "application_pch.h"

#include "window_application/application_test_compute.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/unordered_access.h"
#include "common/draw_system/shader/unordered_access_info.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

namespace
{
    const uint32_t s_num_shader_threads = 8;		// make sure to update value in shader if this changes
    const uint32_t s_tex_width = 1920;
    const uint32_t s_tex_height = 1080;
    const float s_aspect = ((float)s_tex_height / (float)s_tex_width);
}

IWindowApplication* const ApplicationTestCompute::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestCompute(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestCompute::ApplicationTestCompute(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
{
    LOG_MESSAGE(
        "ApplicationTestCompute  ctor %p",
        this
        );
    _draw_system = std::make_unique < DrawSystem > (in_hwnd);

    {
        _draw_resources = std::make_unique<DrawResources>();
        auto command_list = _draw_system->CreateCustomCommandList();

        // std::shared_ptr<UnorderedAccess> _compute_texture;
        {
            std::vector< RenderTargetFormatData > targetFormatDataArray;
            targetFormatDataArray.push_back(RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM));

            const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
                DXGI_FORMAT_R8G8B8A8_UNORM,
                s_tex_width,
                s_tex_height,
                1,
                1,
                1,
                0,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
            );

            D3D12_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc({});
            unordered_access_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            unordered_access_view_desc.Texture2D.MipSlice = 0;
            unordered_access_view_desc.Texture2D.PlaneSlice = 0;

            _draw_resources->_compute_texture = _draw_system->MakeUnorderedAccess(
                command_list->GetCommandList(),
                desc,
                unordered_access_view_desc,
                true
            );
        }

        // std::shared_ptr<Shader> _compute_shader;
        {
            auto pComputeShaderData = FileSystem::SyncReadFile(in_application_param._data_path / "compute_shader.cso");
            ShaderPipelineStateData computePipelineStateData = ShaderPipelineStateData::FactoryComputeShader();

            std::vector< std::shared_ptr< ShaderResourceInfo > > arrayShaderResourceInfo;
            std::vector< std::shared_ptr< ConstantBufferInfo > > arrayShaderConstantsInfo;
            std::vector< std::shared_ptr< UnorderedAccessInfo > > arrayUnorderedAccessInfo;
            arrayUnorderedAccessInfo.push_back(std::make_shared< UnorderedAccessInfo>(
                _draw_resources->_compute_texture->GetHeapWrapperItem()
                ));

            _draw_resources->_compute_shader = _draw_system->MakeShader(
                command_list->GetCommandList(),
                computePipelineStateData,
                nullptr,
                nullptr,
                nullptr,
                arrayShaderResourceInfo,
                arrayShaderConstantsInfo,
                pComputeShaderData,
                arrayUnorderedAccessInfo
            );
        }

#if 1
        // Present
        {
            std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
            input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
                {
                    "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
                });

            // Shader
            {
                auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "present_vertex.cso");
                auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "present_pixel.cso");
                std::vector < DXGI_FORMAT > render_target_format;
                render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

                std::vector< std::shared_ptr< ShaderResourceInfo > > array_shader_resource_info;
                array_shader_resource_info.push_back(ShaderResourceInfo::FactorySampler(
                    _draw_resources->_compute_texture->GetShaderViewHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_PIXEL,
                    true
                ));

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

                _draw_resources->_present_shader = _draw_system->MakeShader(
                    command_list->GetCommandList(),
                    shader_pipeline_state_data,
                    vertex_shader_data,
                    nullptr,
                    pixel_shader_data,
                    array_shader_resource_info
                );
            }

            // Geometry
            {
                const auto vertex_data = VectorHelper::FactoryArrayLiteral(
                    {
                        -1.0f, -1.0f,
                        -1.0f, 1.0f,
                        1.0f, -1.0f,
                        1.0f, 1.0f,
                    }
                );
                _draw_resources->_present_geometry = _draw_system->MakeGeometryGeneric(
                    command_list->GetCommandList(),
                    D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
                    input_element_desc_array,
                    vertex_data,
                    2
                );
            }
        }
#endif

    }
}

ApplicationTestCompute::~ApplicationTestCompute()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }

    _draw_resources.reset();
    _draw_system.reset();

    LOG_MESSAGE(
        "ApplicationTestCompute dtor %p",
        this
        );
}

void ApplicationTestCompute::Update()
{
    BaseType::Update();

    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        // Dispatch compute shader
#if 1
        auto compute_shader = _draw_resources->_compute_shader.get();
        if (nullptr != compute_shader)
        {
            frame->SetShader(compute_shader);

            const int thread_group_count_x = s_tex_width / s_num_shader_threads;
            const int thread_group_count_y = s_tex_height / s_num_shader_threads;

            frame->Dispatch(thread_group_count_x, thread_group_count_y);
        }
#endif

        // Present
#if 1
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        auto present_shader = _draw_resources->_present_shader.get();
        if (nullptr != present_shader)
        {
            frame->ResourceBarrier(
                _draw_resources->_compute_texture.get(), 
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
                );

            frame->SetShader(present_shader);
            frame->Draw(_draw_resources->_present_geometry.get());
        }
#endif
    }
}

void ApplicationTestCompute::OnWindowSizeChanged(
    const int in_width,
    const int in_height
    )
{
    BaseType::OnWindowSizeChanged(
        in_width,
        in_height
        );

    if (_draw_system)
    {
        _draw_system->OnResize();
    }

    return;
}
