#include "application_pch.h"

#include "window_application/application_test_triangle.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/window/window_application_param.h"

IWindowApplication* const ApplicationTestTriangle::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestTriangle(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestTriangle::ApplicationTestTriangle(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
{
    LOG_MESSAGE(
        "ApplicationTestTriangle  ctor %p",
        this
        );
    _draw_system = std::make_unique < DrawSystem > (in_hwnd);
    std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
    }); 
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
    }); 
    auto command_list = _draw_system->CreateCustomCommandList();

    {
        auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "vertex_shader.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "pixel_shader.cso");
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
        _shader = _draw_system->MakeShader(
            command_list->GetCommandList(),
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data
            );
    }

    {
        std::vector < Vertex > vertex_data(
        {
            {
                {
                    0.0f, 0.5f
                },
                {
                    1.0f, 0.0f, 0.0f, 1.0f
                }
            },
            {

                {
                    0.5f, -0.5f
                },
                {
                    0.0f, 1.0f, 0.0f, 1.0f
                }
            },
            {
                {
                    -0.5f, -0.5
                },
                {
                    0.0f, 0.0f, 1.0f, 1.0f
                }
            },
        });
        _geometry = _draw_system->MakeGeometry(
            command_list->GetCommandList(),
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
            input_element_desc_array,
            vertex_data
            );
    }
}

ApplicationTestTriangle::~ApplicationTestTriangle()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _shader.reset();
    _geometry.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationTestTriangle  dtor %p",
        this
        );
}

void ApplicationTestTriangle::Update()
{
    BaseType::Update();
    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());
        frame->SetShader(_shader.get());
        frame->Draw(_geometry.get());
    }
}

void ApplicationTestTriangle::OnWindowSizeChanged(
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

