#include "application_pch.h"

#include "window_application/application_test_text_run.h"
#include "scene_component/scene_component_screen_quad.h"

#include "common/direct_xtk12/keyboard.h"
#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/text/text_run.h"
#include "common/text/text_font.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_manager.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

struct BackgroundConstantBufferB0
{
    int32_t _screen_width;
    int32_t _screen_height;
    int32_t _pad0[2];
};

namespace
{
    static const VectorInt2 s_text_block_size(512, 128);
}

IWindowApplication* const ApplicationTestTextRun::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestTextRun(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestTextRun::ApplicationTestTextRun(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _screen_width(0)
    , _screen_height(0)
{
    LOG_MESSAGE(
        "ApplicationTestTextRun  ctor %p",
        this
        );
    RenderTargetFormatData render_target_format_data(
        DXGI_FORMAT_B8G8R8A8_UNORM
        );
    _draw_system = std::make_unique<DrawSystem>(
        in_hwnd, 
        2, //in_back_buffer_count
        D3D_FEATURE_LEVEL_11_0, 
        0, //in_options
        render_target_format_data
        );
    auto command_list = _draw_system->CreateCustomCommandList();

    _draw_resources = std::make_unique<DrawResources>();

    _draw_resources->_screen_quad = SceneComponentScreenQuad::Factory(
        _draw_system.get(),
        command_list->GetCommandList()
        );

    // Background Shader 
    {
        auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "shader" / "screen_quad_vertex.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "background_pixel.cso");
        std::vector<DXGI_FORMAT> render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        ShaderPipelineStateData shader_pipeline_state_data(
            _draw_resources->_screen_quad->GetInputElementDescArray(),
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
            BackgroundConstantBufferB0(),
            D3D12_SHADER_VISIBILITY_PIXEL
        ));

        _draw_resources->_background_shader = _draw_system->MakeShader(
            command_list->GetCommandList(),
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info,
            array_shader_constants_info
            );
    }

    _draw_resources->_text_manager = std::make_unique<TextManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );
    TextFont* const _text_face = _draw_resources->_text_manager->GetTextFont(
        std::filesystem::path("data") / "code2000.ttf"
        );
#if 0
    const std::vector<std::shared_ptr<TextRunData>> text_run_array({
        std::make_shared<TextRunData>(
            "S",
            nullptr,
            _text_face,
            64,
            72
            ),
        std::make_shared<TextRunData>(
            "S",
            nullptr,
            _text_face,
            32,
            40
            ),
        std::make_shared<TextRunData>(
            "S",
            nullptr,
            _text_face,
            16,
            24
            ),
        std::make_shared<TextRunData>(
            "S",
            nullptr,
            _text_face,
            64,
            72
            ),
        std::make_shared<TextRunData>(
            "S",
            nullptr,
            _text_face,
            32,
            40
            ),
        std::make_shared<TextRunData>(
            "S",
            nullptr,
            _text_face,
            16,
            24
            )
        });
#else
    const std::vector<std::shared_ptr<ITextRunData>> text_run_array({
        TextRun::MakeTextRunDataString(
            "Red",
            nullptr,
            _text_face,
            64,
            72,
            VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f)
            ),
        TextRun::MakeTextRunDataString(
            " Green",
            nullptr,
            _text_face,
            64,
            72,
            VectorFloat4(0.0f, 1.0f, 0.0f, 1.0f)
            ),
        TextRun::MakeTextRunDataString(
            " Blue\n",
            nullptr,
            _text_face,
            64,
            72,
            VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f)
            ),
        TextRun::MakeTextRunDataString(
            "Size64",
            nullptr,
            _text_face,
            64,
            72
            ),
        TextRun::MakeTextRunDataString(
            " Size32",
            nullptr,
            _text_face,
            32,
            40
            ),
        TextRun::MakeTextRunDataString(
            " Size24",
            nullptr,
            _text_face,
            24,
            32
            ),
        TextRun::MakeTextRunDataString(
            " Size16\n",
            nullptr,
            _text_face,
            16,
            24
            ),
        TextRun::MakeTextRunDataString(
            "Translucent",
            nullptr,
            _text_face,
            64,
            72,
            VectorFloat4(0.0f, 0.0f, 0.0f, 0.5f)
            )
    });
#endif
    _draw_resources->_text_run = std::make_unique<TextRun>(
        text_run_array,
        VectorInt2(800, 600),
        false,
        0//,
        //TextEnum::HorizontalLineAlignment::Middle,
        //TextEnum::VerticalBlockAlignment::Middle
        );

    return;
}

ApplicationTestTextRun::~ApplicationTestTextRun()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _draw_resources.reset();
    _draw_system.reset();

    LOG_MESSAGE(
        "ApplicationTestTextRun  dtor %p",
        this
        );
}

void ApplicationTestTextRun::Update()
{
    BaseType::Update();

    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        auto background_shader = _draw_resources->_background_shader.get();
        if (nullptr != background_shader)
        {
            auto& buffer0 = background_shader->GetConstant<BackgroundConstantBufferB0>(0);
            buffer0._screen_width = _screen_width;
            buffer0._screen_height = _screen_height;

            frame->SetShader(background_shader);
            frame->Draw(_draw_resources->_screen_quad->GetGeometry());
        }
        _draw_resources->_text_run->SetTextContainerSize(
            VectorInt2(_screen_width, _screen_height)
            );
        _draw_resources->_text_manager->DrawTextRun(
            _draw_system.get(),
            frame.get(),
            _draw_resources->_text_run.get()
            );
    }
}

void ApplicationTestTextRun::OnWindowSizeChanged(
    const int in_width,
    const int in_height
    )
{
    BaseType::OnWindowSizeChanged(
        in_width,
        in_height
        );

    _screen_width = in_width;
    _screen_height = in_height;

    if (_draw_system)
    {
        _draw_system->OnResize();
    }

    LOG_MESSAGE_INFO("OnWindowSizeChanged %d %d", in_width, in_height);

    return;
}
