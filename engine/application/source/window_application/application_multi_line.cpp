#include "application_pch.h"

#include "window_application/application_multi_line.h"

#include "common/direct_xtk12/keyboard.h"
#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/util/timer.h"
#include "common/window/window_application_param.h"

struct ConstantBufferB0
{
    VectorFloat3 _camera_pos;
    float _camera_fov_horizontal;
    VectorFloat3 _camera_at;
    float _camera_fov_vertical;
    VectorFloat3 _camera_up;
    float _camera_far;
    float _camera_unit_pixel_size; //sin(radian_per_pixel)
    float _pad0[3];
};

struct ConstantBufferBackgroundB1
{
    VectorFloat2 _sun_azimuth_altitude;
    VectorFloat2 _sun_range;
    VectorFloat3 _sun_tint;
    float _sky_spread;
    VectorFloat3 _sky_tint;
    float _sky_turbitity;
    VectorFloat3 _ground_tint;
    float _pad0;
    VectorFloat3 _fog_tint;
    float _pad1;
};

static VectorFloat3 s_camera_pos(-1.0f, 0.0f, 0.0f);
static VectorFloat3 s_camera_at(1.0f, 0.0f, 0.0f);
static VectorFloat3 s_camera_up(0.0f, 1.0f, 0.0f);

IWindowApplication* const ApplicationMultiLine::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationMultiLine(
        in_hwnd,
        in_application_param
        );
}

ApplicationMultiLine::ApplicationMultiLine(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _fov_vertical(Angle::DegToRadian(75.0f))
    , _fov_horizontal_calculated(0.0f)
    , _unit_pixel_size(0.0f)
    , _camera_pos(s_camera_pos)
    , _camera_at(s_camera_at)
    , _camera_up(s_camera_up)
    , _camera_far(1000.0f)
    , _input_q(false)
    , _input_w(false)
    , _input_e(false)
    , _input_a(false)
    , _input_s(false)
    , _input_d(false)
    , _input_u(false)
    , _input_i(false)
    , _input_o(false)
    , _input_j(false)
    , _input_k(false)
    , _input_l(false)
{
    LOG_MESSAGE(
        "ApplicationMultiLine  ctor %p",
        this
        );
    _draw_system = std::make_unique < DrawSystem > (in_hwnd);

    {
        _draw_resources = std::make_unique<DrawResources>();

        std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
        input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
        {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        });
        auto command_list = _draw_system->CreateCustomCommandList();

        // Make shaders
        auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "vertex_shader.cso");

        // Shader background
        {
            auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "pixel_shader_background.cso");
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

            array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                ConstantBufferB0(),
                D3D12_SHADER_VISIBILITY_PIXEL
            ));
            array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                ConstantBufferBackgroundB1(),
                D3D12_SHADER_VISIBILITY_PIXEL
            ));

            _draw_resources->_shader_background = _draw_system->MakeShader(
                command_list->GetCommandList(),
                shader_pipeline_state_data,
                vertex_shader_data,
                nullptr,
                pixel_shader_data,
                array_shader_resource_info,
                array_shader_constants_info
            );
        }

        // Shader grid
        {
            auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "pixel_shader_grid.cso");
            std::vector < DXGI_FORMAT > render_target_format;
            render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
            D3D12_BLEND_DESC blend_desc;
            blend_desc.AlphaToCoverageEnable = FALSE;
            blend_desc.IndependentBlendEnable = FALSE;
            const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
            {
                TRUE, //BlendEnable
                FALSE, //LogicOpEnable
                // dest.rgb = src.rgb * src.a + dest.rgb * (1 - src.a)
                D3D12_BLEND_SRC_ALPHA, //SrcBlend 
                D3D12_BLEND_INV_SRC_ALPHA, //DestBlend
                D3D12_BLEND_OP_ADD, //BlendOp
                // dest.a = 1 - (1 - src.a) * (1 - dest.a) [the math works out]
                D3D12_BLEND_INV_DEST_ALPHA, //SrcBlendAlpha
                D3D12_BLEND_ONE, //DestBlendAlpha
                D3D12_BLEND_OP_ADD, //BlendOpAlpha
                D3D12_LOGIC_OP_NOOP, //LogicOp
                D3D12_COLOR_WRITE_ENABLE_ALL, //RenderTargetWriteMask
            };

            for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
            {
                blend_desc.RenderTarget[i] = defaultRenderTargetBlendDesc;
            }

            ShaderPipelineStateData shader_pipeline_state_data(
                input_element_desc_array,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                DXGI_FORMAT_UNKNOWN,
                // DXGI_FORMAT_D32_FLOAT,
                render_target_format,
                blend_desc, //CD3DX12_BLEND_DESC(D3D12_DEFAULT),
                CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
            );

            std::vector < std::shared_ptr < ShaderResourceInfo > > array_shader_resource_info;
            std::vector < std::shared_ptr < ConstantBufferInfo > > array_shader_constants_info;

            array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                ConstantBufferB0(),
                D3D12_SHADER_VISIBILITY_PIXEL
            ));

            _draw_resources->_shader_grid = _draw_system->MakeShader(
                command_list->GetCommandList(),
                shader_pipeline_state_data,
                vertex_shader_data,
                nullptr,
                pixel_shader_data,
                array_shader_resource_info,
                array_shader_constants_info
            );
        }

        // Geometry Screen Quad
        {
            std::vector < float > vertex_data(
                {
                    -1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f,
                }
            );
            _draw_resources->geometry_screen_quad = _draw_system->MakeGeometryGeneric(
                command_list->GetCommandList(),
                D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
                input_element_desc_array,
                vertex_data,
                2
                );
        }
    }

    _timer = std::make_unique<Timer>();
}

ApplicationMultiLine::~ApplicationMultiLine()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }

    _draw_resources.reset();
    _draw_system.reset();

    LOG_MESSAGE(
        "ApplicationMultiLine  dtor %p",
        this
        );
}

void ApplicationMultiLine::Update()
{
    BaseType::Update();

    if (nullptr != _timer)
    {
        const float time_delta = _timer->GetDeltaSeconds();
        const auto camera_right = Cross(_camera_at, _camera_up);

        //LOG_MESSAGE("ApplicationMultiLine::Update %f %f %f", _camera_pos[0], _camera_pos[1], _camera_pos[2]);

        if (true == _input_q)
        {
            _camera_pos -= (_camera_up * time_delta);
        }
        if (true == _input_w)
        {
            _camera_pos += (_camera_at * time_delta);
        }
        if (true == _input_e)
        {
            _camera_pos += (_camera_up * time_delta);
        }
        if (true == _input_a)
        {
            _camera_pos -= (camera_right * time_delta);
        }
        if (true == _input_s)
        {
            _camera_pos -= (_camera_at * time_delta);
        }
        if (true == _input_d)
        {
            _camera_pos += (camera_right * time_delta);
        }

        bool rotation_flag = false;
        auto rotation = QuaternionFloat::FactoryIdentity();
        if (true == _input_u)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_at, time_delta);
        }
        if (true == _input_i)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(camera_right, -time_delta);
        }
        if (true == _input_o)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_at, -time_delta);
        }
        if (true == _input_j)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_up, time_delta);
        }
        if (true == _input_k)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(camera_right, time_delta);
        }
        if (true == _input_l)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_up, -time_delta);
        }

        if (true == rotation_flag)
        {
            const auto matrix = MatrixFloat33::FactoryQuaternionFloat(rotation);
            //matrix.in
            _camera_at = matrix * _camera_at;
            _camera_up = matrix * _camera_up;

            // Normalise
            _camera_at.NormaliseSelf();
            const auto temp = Cross(_camera_at, _camera_up);
            _camera_up = Cross(temp, _camera_at);
            _camera_up.NormaliseSelf();
        }
    }

    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        // Draw background
#if 1
        auto shader_background = _draw_resources->_shader_background.get();
        if (nullptr != shader_background)
        {
            auto& buffer0 = shader_background->GetConstant<ConstantBufferB0>(0);
            buffer0._camera_at = _camera_at;
            buffer0._camera_up = _camera_up;
            buffer0._camera_pos = _camera_pos;
            buffer0._camera_far = _camera_far;
            buffer0._camera_fov_horizontal = _fov_horizontal_calculated;
            buffer0._camera_fov_vertical = _fov_vertical;
            buffer0._camera_unit_pixel_size = _unit_pixel_size;

            auto& buffer_background1 = shader_background->GetConstant<ConstantBufferBackgroundB1>(1);

            buffer_background1._sun_azimuth_altitude = VectorFloat2(Angle::DegToRadian(45.0f), Angle::DegToRadian(45.0f));
            buffer_background1._sun_range = VectorFloat2(0.05f, 0.2f);//1.0f, 5.0f);
            buffer_background1._sun_tint = VectorFloat3(255.0f / 255.0f, 245.0f / 255.0f, 235.0f / 255.0f);
            buffer_background1._sky_spread = -0.75f; //-0.5
            buffer_background1._sky_tint = VectorFloat3(10.0f / 255.0f, 10.0f / 255.0f, 255.0f / 255.0f);
            buffer_background1._sky_turbitity = 25.0f; //5.0f; //10.0f;
            buffer_background1._ground_tint = VectorFloat3(32.0f / 255.0f, 16.0f / 255.0f, 2.0f / 255.0f);
            buffer_background1._fog_tint = VectorFloat3(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f);
        }
        frame->SetShader(shader_background);
        frame->Draw(_draw_resources->geometry_screen_quad.get());
#endif

        // Draw Grid
#if 1
        auto shader_grid = _draw_resources->_shader_grid.get();
        if (nullptr != shader_grid)
        {
            auto& buffer0 = shader_grid->GetConstant<ConstantBufferB0>(0);
            buffer0._camera_at = _camera_at;
            buffer0._camera_up = _camera_up;
            buffer0._camera_pos = _camera_pos;
            buffer0._camera_far = _camera_far;
            buffer0._camera_fov_horizontal = _fov_horizontal_calculated;
            buffer0._camera_fov_vertical = _fov_vertical;
            buffer0._camera_unit_pixel_size = _unit_pixel_size;

            frame->SetShader(shader_grid);
            frame->Draw(_draw_resources->geometry_screen_quad.get());
        }
#endif

    }
}

void ApplicationMultiLine::OnWindowSizeChanged(
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
    const auto radian_per_pixel = _fov_vertical / (float)in_height;
    _unit_pixel_size = sin(radian_per_pixel);

    if (_draw_system)
    {
        _draw_system->OnResize();
    }

    return;
}

void ApplicationMultiLine::OnKey(
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

    if ('Q' == in_vk_code)
    {
        _input_q = (false == in_up_flag);
    }
    else if ('W' == in_vk_code)
    {
        _input_w = (false == in_up_flag);
    }
    else if ('E' == in_vk_code)
    {
        _input_e = (false == in_up_flag);
    }
    else if ('A' == in_vk_code)
    {
        _input_a = (false == in_up_flag);
    }
    else if ('S' == in_vk_code)
    {
        _input_s = (false == in_up_flag);
    }
    else if ('D' == in_vk_code)
    {
        _input_d = (false == in_up_flag);
    }

    else if ('U' == in_vk_code)
    {
        _input_u = (false == in_up_flag);
    }
    else if ('I' == in_vk_code)
    {
        _input_i = (false == in_up_flag);
    }
    else if ('O' == in_vk_code)
    {
        _input_o = (false == in_up_flag);
    }
    else if ('J' == in_vk_code)
    {
        _input_j = (false == in_up_flag);
    }
    else if ('K' == in_vk_code)
    {
        _input_k = (false == in_up_flag);
    }
    else if ('L' == in_vk_code)
    {
        _input_l = (false == in_up_flag);
    }
    else if ((VK_HOME == in_vk_code) && (false == in_up_flag))
    {
        _camera_pos = s_camera_pos;
        _camera_at = s_camera_at;
        _camera_up = s_camera_up;
    }

    return;
}
