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
        "ApplicationTestDescriptor  ctor %p",
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
        DXGI_FORMAT_R8G8B8A8_SNORM, //DXGI_FORMAT Format;
        DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
        D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
        D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //D3D12_RESOURCE_FLAGS Flags;
        };
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Format = desc.Format;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    view_desc.Texture2D.MipLevels = 1;

    auto heap_wrapper_texture_0 = _draw_system->MakeHeapWrapperCbvSrvUav();
    {
        std::vector<uint8_t> data;
        text_font->DrawToPixels(
            data,
            64,
            64,
            "a",
            _draw_resource->_text_manager->GetLocaleToken(LocaleISO_639_1::English),
            32
            );

        _draw_resource->_texture_0 = _draw_system->MakeShaderResource(
            command_list->GetCommandList(),
            heap_wrapper_texture_0,
            desc,
            view_desc,
            data);
    }

#if 0
    auto heap_wrapper_texture_1 = _draw_system->MakeHeapWrapperCbvSrvUav();
    {
        std::vector<uint8_t> data;
        text_font->DrawToPixels(
            data,
            64,
            64,
            "b",
            _text_manager->GetLocaleToken(LocaleISO_639_1::English),
            32
            );

        _draw_resource->_texture_1 = _draw_system->MakeShaderResource(
            command_list->GetCommandList(),
            heap_wrapper_texture_1,
            desc,
            view_desc,
            data);
    }

    auto heap_wrapper_texture_2 = _draw_system->MakeHeapWrapperCbvSrvUav();
    {
        std::vector<uint8_t> data;
        text_font->DrawToPixels(
            data,
            64,
            64,
            "c",
            _text_manager->GetLocaleToken(LocaleISO_639_1::English),
            32
            );

        _draw_resource->_texture_2 = _draw_system->MakeShaderResource(
            command_list->GetCommandList(),
            heap_wrapper_texture_2,
            desc,
            view_desc,
            data);
    }

    auto heap_wrapper_texture_3 = _draw_system->MakeHeapWrapperCbvSrvUav();
    {
        std::vector<uint8_t> data;
        text_font->DrawToPixels(
            data,
            64,
            64,
            "d",
            _text_manager->GetLocaleToken(LocaleISO_639_1::English),
            32
            );

        _draw_resource->_texture_3 = _draw_system->MakeShaderResource(
            command_list->GetCommandList(),
            heap_wrapper_texture_3,
            desc,
            view_desc,
            data);
    }

    auto heap_wrapper_texture_4 = _draw_system->MakeHeapWrapperCbvSrvUav();
    {
        std::vector<uint8_t> data;
        text_font->DrawToPixels(
            data,
            64,
            64,
            "e",
            _text_manager->GetLocaleToken(LocaleISO_639_1::English),
            32
            );

        _draw_resource->_texture_4 = _draw_system->MakeShaderResource(
            command_list->GetCommandList(),
            heap_wrapper_texture_4,
            desc,
            view_desc,
            data);
    }
#endif //#if 0

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

        array_shader_resource_info.push_back(ShaderResourceInfo::FactorySampler(
            _draw_resource->_texture_0->GetHeapWrapperItem(),
            D3D12_SHADER_VISIBILITY_PIXEL
            ));
#if 0
        array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
            heap_wrapper_texture_1,
            D3D12_SHADER_VISIBILITY_PIXEL
            ));
        array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
            heap_wrapper_texture_2,
            D3D12_SHADER_VISIBILITY_PIXEL
            ));
        array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
            heap_wrapper_texture_3,
            D3D12_SHADER_VISIBILITY_PIXEL
            ));
        array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
            heap_wrapper_texture_4,
            D3D12_SHADER_VISIBILITY_PIXEL
            ));
#endif //#if 0

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
