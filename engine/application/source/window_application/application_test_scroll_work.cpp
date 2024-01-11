#include "application_pch.h"

#include "window_application/application_test_scroll_work.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

struct ConstantBufferB0
{
    VectorFloat2 _width_height; // pixels
};

IWindowApplication* const ApplicationTestScrollWork::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestScrollWork(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestScrollWork::ApplicationTestScrollWork(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
{
    LOG_MESSAGE(
        "ApplicationTestScrollWork  ctor %p",
        this
        );
    RenderTargetFormatData render_target_format_data(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        true,
        VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    _draw_system = std::make_unique < DrawSystem > (
        in_hwnd, 
        2, //in_back_buffer_count
        D3D_FEATURE_LEVEL_11_0, 
        0, //in_options
        render_target_format_data
        );
    std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
    });
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
    });
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "COLOR", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
    });
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "COLOR", 2, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
    });


    auto command_list = _draw_system->CreateCustomCommandList();

    // Make shader
    {
        auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "vertex_shader.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "pixel_shader.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
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

        std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
        std::vector<std::shared_ptr<ConstantBufferInfo>> array_shader_constants_info;
        array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
            ConstantBufferB0(),
            D3D12_SHADER_VISIBILITY_ALL
            ));

        _shader = _draw_system->MakeShader(
            command_list->GetCommandList(),
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info,
            array_shader_constants_info
        );

        _shader_constant_buffer = _shader->MakeShaderConstantBuffer(_draw_system.get());

    }

    // Geometry Screen Quad
    {
        const auto vertex_data = VectorHelper::FactoryArrayLiteral(
            {
                -1.0f, -1.0f, 0.25f, 0.25f, 0.5f, 0.75f, 0.75f, 0.25f,
                -1.0f, 1.0f,  0.25f, 0.25f, 0.5f, 0.75f, 0.75f, 0.25f,
                1.0f, -1.0f,  0.25f, 0.25f, 0.5f, 0.75f, 0.75f, 0.25f,
                1.0f, 1.0f,   0.25f, 0.25f, 0.5f, 0.75f, 0.75f, 0.25f,
            }
        );
        _geometry = _draw_system->MakeGeometryGeneric(
            command_list->GetCommandList(),
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            input_element_desc_array,
            vertex_data,
            8
            );
    }
}

ApplicationTestScrollWork::~ApplicationTestScrollWork()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _shader_constant_buffer.reset();
    _shader.reset();
    _geometry.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationTestScrollWork  dtor %p",
        this
        );
}

void ApplicationTestScrollWork::Update()
{
    BaseType::Update();

    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        // Draw
        const VectorInt2 backbufferSize = _draw_system->GetRenderTargetBackBuffer()->GetSize();
        auto& buffer0 = _shader_constant_buffer->GetConstant<ConstantBufferB0>(0);
        buffer0._width_height[0] = static_cast<float>(backbufferSize[0]);
        buffer0._width_height[1] = static_cast<float>(backbufferSize[1]);

        frame->SetShader(_shader, _shader_constant_buffer);
        frame->Draw(_geometry);
    }
}

void ApplicationTestScrollWork::OnWindowSizeChanged(
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

void ApplicationTestScrollWork::OnKey(
    const int in_vk_code,
    const int in_scan_code,
    const bool in_repeat_flag,
    const int in_repeat_count,
    bool in_up_flag
    )
{
    in_vk_code;
    in_scan_code;
    in_repeat_flag;
    in_repeat_count;
    in_up_flag;

    return;
}
