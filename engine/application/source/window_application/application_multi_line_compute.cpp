#include "application_pch.h"

#include "window_application/application_multi_line_compute.h"

#include "scene_component/scene_component_background.h"
#include "scene_component/scene_component_camera_ray.h"
#include "scene_component/scene_component_screen_quad.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
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

struct ConstantBufferComputeB0
{
    float _timer;
    float _thickness;
    float _pad0[2];
    VectorFloat4 _colour;
};

namespace
{
    static int s_thread_x = 8;
    static int s_thread_y = 8;
    static int s_thread_z = 1;
    static int s_texture_width = 8 * 24;
    static int s_texture_height = 8;
    static int s_line_segment_count = s_texture_width * s_texture_height;

    static std::shared_ptr<UnorderedAccess> MakeUnordedAccess(
        DrawSystem* in_draw_system,
        ID3D12GraphicsCommandList* in_command_list
        )
    {
        std::vector< RenderTargetFormatData > targetFormatDataArray;
        targetFormatDataArray.push_back(RenderTargetFormatData(DXGI_FORMAT_R32G32B32A32_FLOAT));

        const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            s_texture_width,
            s_texture_height,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
        );

        D3D12_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc({});
        unordered_access_view_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        unordered_access_view_desc.Texture2D.MipSlice = 0;
        unordered_access_view_desc.Texture2D.PlaneSlice = 0;

        return in_draw_system->MakeUnorderedAccess(
            in_command_list,
            desc,
            unordered_access_view_desc,
            true
        );
    }

}

IWindowApplication* const ApplicationMultiLineCompute::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationMultiLineCompute(
        in_hwnd,
        in_application_param
        );
}

ApplicationMultiLineCompute::ApplicationMultiLineCompute(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _timer_accumulate_wrapped(0.0f)
{
    LOG_MESSAGE(
        "ApplicationMultiLineCompute  ctor %p",
        this
        );
    _draw_system = std::make_unique < DrawSystem > (in_hwnd);

    {
        _draw_resources = std::make_unique<DrawResources>();
        auto command_list = _draw_system->CreateCustomCommandList();

        _draw_resources->_screen_quad = SceneComponentScreenQuad::Factory(
            _draw_system.get(),
            command_list->GetCommandList()
            );

        _draw_resources->_camera_ray = SceneComponentCameraRay::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path
            );

        _draw_resources->_background = SceneComponentBackground::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path,
            _draw_resources->_camera_ray->GetHeapWrapperCameraRay()
            );

        _draw_resources->_multi_line_data_uv_low_uv_high = MakeUnordedAccess(
            _draw_system.get(),
            command_list->GetCommandList()
            );
        _draw_resources->_multi_line_data_pos_thick = MakeUnordedAccess(
            _draw_system.get(),
            command_list->GetCommandList()
            );
        _draw_resources->_multi_line_data_dir_length = MakeUnordedAccess(
            _draw_system.get(),
            command_list->GetCommandList()
            );
        _draw_resources->_multi_line_data_colour = MakeUnordedAccess(
            _draw_system.get(),
            command_list->GetCommandList()
            );

        // Compute shader
        {
            auto pComputeShaderData = FileSystem::SyncReadFile(in_application_param._data_path / "multi_line_compute.cso");
            ShaderPipelineStateData computePipelineStateData = ShaderPipelineStateData::FactoryComputeShader();

            std::vector< std::shared_ptr< ShaderResourceInfo > > array_shader_resource_info;
            std::vector< std::shared_ptr< ConstantBufferInfo > > array_shader_constants_info;
            array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                CameraConstantBufferB0(),
                D3D12_SHADER_VISIBILITY_ALL
                ));

            std::vector< std::shared_ptr<UnorderedAccessInfo>> array_unordered_access_info;
            array_unordered_access_info.push_back(std::make_shared<UnorderedAccessInfo>(
                _draw_resources->_multi_line_data_uv_low_uv_high->GetHeapWrapperItem()
                ));
            array_unordered_access_info.push_back(std::make_shared<UnorderedAccessInfo>(
                _draw_resources->_multi_line_data_pos_thick->GetHeapWrapperItem()
                ));
/*
            array_unordered_access_info.push_back(std::make_shared<UnorderedAccessInfo>(
                _draw_resources->_multi_line_data_dir_length->GetHeapWrapperItem()
                ));
            array_unordered_access_info.push_back(std::make_shared<UnorderedAccessInfo>(
                _draw_resources->_multi_line_data_colour->GetHeapWrapperItem()
                ));
*/
            _draw_resources->_multi_line_compute = _draw_system->MakeShader(
                command_list->GetCommandList(),
                computePipelineStateData,
                nullptr,
                nullptr,
                nullptr,
                array_shader_resource_info,
                array_shader_constants_info,
                pComputeShaderData,
                array_unordered_access_info
                );
            _draw_resources->_multi_line_compute_constant_buffer = _draw_resources->_multi_line_compute->MakeShaderConstantBuffer(_draw_system.get());
        }

        // Multi line shader and geometry
        {
            std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
            input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
                {
                    "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
                });
            input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
                {
                    "TEXCOORD", 0, DXGI_FORMAT_R32G32_SINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
                });

            // std::shared_ptr<Shader> _multi_line_shader;
            {
                auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "multi_line_vertex.cso");
                auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "multi_line_pixel.cso");
                std::vector < DXGI_FORMAT > render_target_format;
                render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
                const auto blend_desc = ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied();
                ShaderPipelineStateData shader_pipeline_state_data(
                    input_element_desc_array,
                    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                    DXGI_FORMAT_UNKNOWN,
                    // DXGI_FORMAT_D32_FLOAT,
                    render_target_format,
                    blend_desc,
                    CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                    CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
                );

                std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;

                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_uv_low_uv_high->GetShaderViewHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_pos_thick->GetShaderViewHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
                //array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                //    _draw_resources->_multi_line_data_dir_length->GetShaderViewHeapWrapperItem(),
                //    D3D12_SHADER_VISIBILITY_VERTEX
                //));
                //array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                //    _draw_resources->_multi_line_data_colour->GetShaderViewHeapWrapperItem(),
                //    D3D12_SHADER_VISIBILITY_VERTEX
                //));
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_camera_ray->GetHeapWrapperCameraRay(),
                    D3D12_SHADER_VISIBILITY_PIXEL
                ));

                std::vector<std::shared_ptr<ConstantBufferInfo>> array_shader_constants_info;
                array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                    CameraConstantBufferB0(),
                    D3D12_SHADER_VISIBILITY_PIXEL
                    ));

                _draw_resources->_multi_line_shader = _draw_system->MakeShader(
                    command_list->GetCommandList(),
                    shader_pipeline_state_data,
                    vertex_shader_data,
                    nullptr,
                    pixel_shader_data,
                    array_shader_resource_info,
                    array_shader_constants_info
                    );
                _draw_resources->_multi_line_shader_constant_buffer = _draw_resources->_multi_line_shader->MakeShaderConstantBuffer(_draw_system.get());
            }

            // std::shared_ptr<GeometryGeneric> _multi_line_geometry;
            {
                std::vector<uint8_t> vertex_data;
                int trace = 0;
                for (int y = 0; y < s_texture_height; ++y)
                {
                    for (int x = 0; x < s_texture_width; ++x)
                    {
                        ++trace;
                        if (s_line_segment_count < trace)
                        {
                            break;
                        }
                        //a
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //b
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //c
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);

                        //c
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //b
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //d
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                    }
                    if (s_line_segment_count < trace)
                    {
                        break;
                    }
                }
                _draw_resources->_multi_line_geometry = _draw_system->MakeGeometryGeneric(
                    command_list->GetCommandList(),
                    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                    input_element_desc_array,
                    vertex_data,
                    4
                );
            }
        }
    }

    _timer = std::make_unique<Timer>();
}

ApplicationMultiLineCompute::~ApplicationMultiLineCompute()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }

    _draw_resources.reset();
    _draw_system.reset();

    LOG_MESSAGE(
        "ApplicationMultiLineCompute  dtor %p",
        this
        );
}

void ApplicationMultiLineCompute::Update()
{
    BaseType::Update();

    const float time_delta = _timer ? _timer->GetDeltaSeconds() : 0.0f;

    if (_draw_system && _draw_resources)
    {
        auto frame = _draw_system->CreateNewFrame();
        auto screen_quad = _draw_resources->_screen_quad ? _draw_resources->_screen_quad->GetGeometry() : nullptr;

        if (_draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_camera_ray->Update(
                time_delta,
                frame.get(),
                screen_quad
                );
        }

        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());
        if (_draw_resources->_background && _draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_background->Draw(
                _draw_system.get(),
                frame.get(),
                screen_quad,
                _draw_resources->_camera_ray->GetConstantBufferB0()
                );
        }

        // Draw Multi Line Compute
#if 1
        auto multi_line_compute = _draw_resources->_multi_line_compute.get();
        auto multi_line_compute_constant_buffer = _draw_resources->_multi_line_compute_constant_buffer.get();
        if ((nullptr != multi_line_compute) && (nullptr != multi_line_compute_constant_buffer))
        {
            frame->ResourceBarrier(_draw_resources->_multi_line_data_uv_low_uv_high.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_pos_thick.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_dir_length.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_colour.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

            multi_line_compute_constant_buffer->GetConstant<CameraConstantBufferB0>(0) = _draw_resources->_camera_ray->GetConstantBufferB0();

            frame->SetShader(multi_line_compute, multi_line_compute_constant_buffer);
            frame->Dispatch(
                s_texture_width / s_thread_x,
                s_texture_height / s_thread_y
                );
        }
#endif

        // Draw Multi Line
#if 1
        auto multi_line_shader = _draw_resources->_multi_line_shader.get();
        auto multi_line_shader_constant_buffer = _draw_resources->_multi_line_shader_constant_buffer.get();
        if ((nullptr != multi_line_shader) && (nullptr != multi_line_shader_constant_buffer))
        {
            frame->ResourceBarrier(_draw_resources->_multi_line_data_uv_low_uv_high.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_pos_thick.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_dir_length.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_colour.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

            multi_line_shader_constant_buffer->GetConstant<CameraConstantBufferB0>(0) = _draw_resources->_camera_ray->GetConstantBufferB0();

            frame->SetShader(multi_line_shader, multi_line_shader_constant_buffer);
            frame->Draw(_draw_resources->_multi_line_geometry.get());
        }
#endif

    }
}

void ApplicationMultiLineCompute::OnWindowSizeChanged(
    const int in_width,
    const int in_height
    )
{
    BaseType::OnWindowSizeChanged(
        in_width,
        in_height
        );

    if (_draw_resources && _draw_resources->_camera_ray)
    {
        _draw_resources->_camera_ray->OnWindowSizeChanged(
            in_width,
            in_height
            );
    }

    if (_draw_system)
    {
        _draw_system->OnResize();
    }

    return;
}

void ApplicationMultiLineCompute::OnKey(
    const int in_vk_code,
    const int in_scan_code,
    const bool in_repeat_flag,
    const int in_repeat_count,
    bool in_up_flag
    )
{
    in_scan_code;
    in_repeat_flag;
    in_repeat_count;

    if (_draw_resources && _draw_resources->_camera_ray)
    {
        _draw_resources->_camera_ray->OnKey(
            in_vk_code,
            in_up_flag
            );
    }

    return;
}
