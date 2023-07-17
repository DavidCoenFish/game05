#include "application_pch.h"

#include "window_application/application_test_line.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/window/window_application_param.h"

struct ConstantBufferB0
{
    VectorFloat3 _camera_pos;
    float _camera_fov_horizontal;
    VectorFloat3 _camera_at;
    float _camera_fov_vertical;
    VectorFloat3 _camera_up;
    float _camera_far;

    VectorFloat3 _line_pos;
    float _radian_per_pixel;
    VectorFloat3 _line_at;
    float _line_length;

};

IWindowApplication* const ApplicationTestLine::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestLine(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestLine::ApplicationTestLine(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _fov_vertical(Angle::DegToRadian(75.0f))
    , _fov_horizontal_calculated(0.0f)
    , _camera_pos(1.0f, 0.0f, -1.0f)
    , _camera_at(0.0f, 0.0f, 1.0f)
    , _camera_up(0.0f, 1.0f, 0.0f)
    , _camera_far(1000.0f)
    , _line_pos(0.0f, 0.0f, 0.0f)
    , _line_at(0.0f, 1.0f, 0.0f)
    , _line_length(2.0f)
{
    LOG_MESSAGE(
        "ApplicationTestLine  ctor %p",
        this
        );
    _draw_system = std::make_unique < DrawSystem > (in_hwnd);
    std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
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

        std::vector < std::shared_ptr < ShaderResourceInfo > > array_shader_resource_info;
        std::vector < std::shared_ptr < ConstantBufferInfo > > array_shader_constants_info;

        array_shader_constants_info.push_back(ConstantBufferInfo::Factory< ConstantBufferB0>(
            ConstantBufferB0(), 
            D3D12_SHADER_VISIBILITY_PIXEL
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
    }

    {
        std::vector < float > vertex_data(
            {
                -1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, -1.0f,
                1.0f, 1.0f,
            }
        );
        _geometry = _draw_system->MakeGeometryGeneric(
            command_list->GetCommandList(),
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            input_element_desc_array,
            vertex_data,
            2
            );
    }
}

ApplicationTestLine::~ApplicationTestLine()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _shader.reset();
    _geometry.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationTestLine  dtor %p",
        this
        );
}

void ApplicationTestLine::Update()
{
    BaseType::Update();
    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        auto shader = _shader.get();
        if (nullptr != shader)
        {
            auto& buffer = shader->GetConstant<ConstantBufferB0>(0);
            buffer._camera_at = _camera_at;
            buffer._camera_up = _camera_up;
            buffer._camera_pos = _camera_pos;
            buffer._camera_far = _camera_far;
            buffer._camera_fov_horizontal = _fov_horizontal_calculated;
            buffer._camera_fov_vertical = _fov_vertical;
            buffer._line_at = _line_at;
            buffer._line_pos = _line_pos;
            buffer._radian_per_pixel = _radian_per_pixel;
            buffer._line_length = _line_length;
        }

        frame->SetShader(shader);
        frame->Draw(_geometry.get());
    }
}

void ApplicationTestLine::OnWindowSizeChanged(
    const int in_width,
    const int in_height
    )
{
    BaseType::OnWindowSizeChanged(
        in_width,
        in_height
        );

    const float aspect = (float)in_width / (float)in_height;
    _fov_horizontal_calculated = aspect * _fov_vertical;
    _radian_per_pixel = _fov_vertical / (float)in_height;

    if (_draw_system)
    {
        _draw_system->OnResize();
    }

    return;
}

