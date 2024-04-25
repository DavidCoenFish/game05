#include "application_pch.h"

#include "window_application/application_test_descriptor.h"
#include "scene_component/scene_component_screen_quad.h"

#include "common/bezier/bezier_border_helper.h"
#include "common/bezier/bezier_curve.h"
#include "common/bezier/bezier_file_helper.h"
#include "common/bezier/bezier_manager.h"
#include "common/bezier/bezier_simulate.h"
#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/locale/locale_enum.h"
#include "common/math/angle.h"
#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/text/text_run.h"
#include "common/text/text_font.h"
#include "common/text/text_manager.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

namespace
{
    const std::shared_ptr<ShaderResource> MakeTexture(
        CustomCommandList& in_command_list,
        const std::string& in_text,
        const D3D12_RESOURCE_DESC& in_desc,
        const D3D12_SHADER_RESOURCE_VIEW_DESC& in_view_desc,
        DrawSystem& in_draw_system,
        const TextLocale* const in_text_locale,
        TextFont* const in_text_font,
        const int in_offset_x,
        const int in_offset_y
        )
    {
        auto heap_wrapper_texture = in_draw_system.MakeHeapWrapperCbvSrvUav();
        std::vector<uint8_t> data;
        in_text_font->DrawToPixels(
            data,
            static_cast<int>(in_desc.Width),
            static_cast<int>(in_desc.Height),
            in_text,
            in_text_locale,
            40,
            in_offset_x,
            in_offset_y
            );

        auto texture = in_draw_system.MakeShaderResource(
            in_command_list.GetCommandList(),
            heap_wrapper_texture,
            in_desc,
            in_view_desc,
            data);
        return texture;
    }

};

IWindowApplication* const ApplicationTestDescriptor::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestDescriptor(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestDescriptor::ApplicationTestDescriptor(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
{
    LOG_MESSAGE(
        "ApplicationTestDescriptor ctor %p",
        this
        );
    RenderTargetFormatData render_target_format_data(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        true,
        VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    _draw_system = std::make_unique<DrawSystem>(
        in_hwnd, 
        2, //in_back_buffer_count
        D3D_FEATURE_LEVEL_11_0, 
        0, //in_options
        render_target_format_data,
        RenderTargetDepthData(),
        8 //256 // heap descripters per page
        );

    auto command_list = _draw_system->CreateCustomCommandList();

    _draw_resource = std::make_unique<DrawResource>();

    _draw_resource->_screen_quad = SceneComponentScreenQuad::Factory(
        _draw_system.get(),
        command_list->GetCommandList()
        );

    _draw_resource->_text_manager = std::make_unique<TextManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );
    TextFont* const text_font = _draw_resource->_text_manager->GetTextFont(
        std::filesystem::path("data") / "code2000.ttf"
        );

    D3D12_RESOURCE_DESC desc = {
        D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
        D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
        64, //UINT64 Width;
        64, //UINT Height;
        1, //UINT16 DepthOrArraySize;
        1, //UINT16 MipLevels;
        DXGI_FORMAT_R8G8B8A8_UNORM, //DXGI_FORMAT Format;
        DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
        D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
        D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //D3D12_RESOURCE_FLAGS Flags;
        };
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Format = desc.Format;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    view_desc.Texture2D.MipLevels = 1;

    const TextLocale* const text_locale = _draw_resource->_text_manager->GetLocaleToken(LocaleISO_639_1::English);
    const std::string char_data[Var::TTextureCount] = {"a", "b", "c", "d", "e", "f", "g", "h", "i"};
    const int offset_x[Var::TTextureCount] = {0, 20, 40, 0, 20, 40, 0, 20, 40};
    const int offset_y[Var::TTextureCount] = {0, 0, 0, 20, 20, 20, 40, 40, 40};
    for (int index = 0; index < Var::TTextureCount; ++index)
    {
        _draw_resource->_texture[index] = MakeTexture(*command_list, char_data[index], desc, view_desc, *_draw_system, text_locale, text_font, offset_x[index], offset_y[index]);
    }

    {
        auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "shader" / "screen_quad_vertex.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "pixel_shader.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        const auto blend_desc = ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied();
        ShaderPipelineStateData shader_pipeline_state_data(
            _draw_resource->_screen_quad->GetInputElementDescArray(),
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN,
            render_target_format,
            blend_desc,
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC()
            );
        std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
        for (int index = 0; index < Var::TTextureCount; ++index)
        {
            array_shader_resource_info.push_back(ShaderResourceInfo::FactoryDataSampler(
                _draw_resource->_texture[index]->GetHeapWrapperItem(),
                D3D12_SHADER_VISIBILITY_PIXEL
                ));
        }

        _draw_resource->_shader = _draw_system->MakeShader(
            command_list->GetCommandList(),
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info
            );
    }


}

ApplicationTestDescriptor::~ApplicationTestDescriptor()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _draw_resource.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationTestDescriptor  dtor %p",
        this
        );
}

void ApplicationTestDescriptor::Update()
{
    BaseType::Update();

    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        if (_draw_resource && _draw_resource->_shader && _draw_resource->_screen_quad)
        {
            frame->SetShader(_draw_resource->_shader);
            frame->Draw(_draw_resource->_screen_quad->GetGeometryRef());
        }
    }
}

void ApplicationTestDescriptor::OnWindowSizeChanged(
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
