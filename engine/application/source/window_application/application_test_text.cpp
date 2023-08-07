#include "application_pch.h"

#include "window_application/application_test_text.h"
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
#include "common/text/text_block.h"
#include "common/text/text_face.h"
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

IWindowApplication* const ApplicationTestText::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestText(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestText::ApplicationTestText(
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
        "ApplicationTestText  ctor %p",
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

    _draw_resources->_ui_manager = std::make_shared<UIManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );

    _text_manager = std::make_shared<TextManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );

    _draw_resources->_text_face = _text_manager->MakeTextFace(
        in_application_param._root_path / "data" / "open_sans.ttf"
        );

    _draw_resources->_text_block = _draw_resources->_text_face->MakeBlock(
        _draw_system.get(),
        command_list->GetCommandList(),
        "abcdefg",
        nullptr,
        64,
        VectorInt2(800, 600), //s_text_block_size,
        false, //const bool in_width_limit_enabled,
        0, //const int in_width_limit,
        TextEnum::HorizontalLineAlignment::Middle,
        TextEnum::VerticalBlockAlignment::Middle
        );

    const std::vector<RenderTargetFormatData> target_format_data_array({
        RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM)
        });

    _draw_resources->_ui_render_target = _draw_system->MakeRenderTargetTexture(
        command_list->GetCommandList(),
        target_format_data_array,
        RenderTargetDepthData(),
        VectorInt2(800,600) // s_text_block_size
        );

    _draw_resources->_ui_geometry = _draw_resources->_ui_manager->GeometryHelper(
        _draw_system.get(),
        command_list->GetCommandList(),
        VectorFloat4(
            (-800.0f + 64.0f) / 800.0f,
            (-600.0f + 64.0f) / 600.0f,
            (-800.0f + 64.0f + 512.0f) / 800.0f,
            (-600.0f + 64.0f + 128.0f) / 600.0f
            )
        );
#if 0
    _draw_resources->_ui_block = std::make_shared<UiBlock>(
        _draw_system.get(),
        command_list->GetCommandList(),
        s_text_block_size,
        true,
        VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f)
        );
#endif

    return;
}

ApplicationTestText::~ApplicationTestText()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _draw_resources.reset();
    _text_manager.reset();
    _draw_system.reset();

    LOG_MESSAGE(
        "ApplicationTestText  dtor %p",
        this
        );
}

void ApplicationTestText::Update()
{
    BaseType::Update();

    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

#if 1
        auto background_shader = _draw_resources->_background_shader.get();
        if (nullptr != background_shader)
        {
            auto& buffer0 = background_shader->GetConstant<BackgroundConstantBufferB0>(0);
            buffer0._screen_width = _screen_width;
            buffer0._screen_height = _screen_height;

            frame->SetShader(background_shader);
            frame->Draw(_draw_resources->_screen_quad->GetGeometry());
        }
#endif
        _draw_resources->_text_block->SetTextContainerSize(
            VectorInt2(_screen_width, _screen_height)
            );
        //_draw_resources->_text_block->Update(
        //    _draw_system.get(),
        //    frame->GetCommandList()
        //    );
        _text_manager->DrawText(
            _draw_system.get(),
            frame.get(),
            _draw_resources->_text_block.get()
            );


//        frame->SetRenderTarget(_draw_resources->_ui_render_target.get());
//        _text_manager->DrawText(
//            _draw_system.get(),
//            frame.get(),
//            _draw_resources->_text_block.get()
//            );
//        //frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());
//
//        _draw_resources->_ui_manager->DrawHelper(
//            frame.get(),
//            _draw_system->GetRenderTargetBackBuffer(),
//            _draw_resources->_ui_geometry.get(),
//            _draw_resources->_ui_render_target->GetShaderResourceHeapWrapperItem()
//            );
//
//#if 0
//        _draw_resources->_ui_block->Activate(frame.get());
//        _text_manager->DrawText(
//            _draw_system.get(),
//            frame.get(),
//            _draw_resources->_text_block.get()
//            );
//
//        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());
//        auto texture = _draw_resources->_ui_block->GetTexture(frame.get());
//        _draw_resources->_ui_manager->DrawBlock(
//            frame.get(),
//            VectorInt2(_screen_width, _screen_height),
//            VectorInt4(128, 64, s_text_block_size.GetX() + 128, s_text_block_size.GetY() + 64),
//            VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f),
//            texture
//            );
//#endif
    }
}

void ApplicationTestText::OnWindowSizeChanged(
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
