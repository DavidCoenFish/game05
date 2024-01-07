#include "application_pch.h"

#include "window_application/application_test_svg.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/util/vector_helper.h"
#include "common/util/utf8.h"
#include "common/window/window_application_param.h"

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/include/nanosvg.h"
#include "nanosvg/include/nanosvgrast.h"

IWindowApplication* const ApplicationTestSvg::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestSvg(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestSvg::ApplicationTestSvg(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
{
    LOG_MESSAGE(
        "ApplicationTestSvg  ctor %p",
        this
        );
    _draw_system = std::make_unique<DrawSystem> (in_hwnd);
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_element_desc_array;
    input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
    {
        "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
    });
    auto command_list = _draw_system->CreateCustomCommandList();
    {
        NSVGimage *image = NULL;
        NSVGrasterizer *rast = NULL;
        int w = 0;
        int h = 0;
        std::vector<uint8_t> data;

        const std::filesystem::path svg_file_path = in_application_param._root_path / "data" / "example.svg";

        image = nsvgParseFromFile(Utf8::Utf16ToUtf8(svg_file_path).c_str(), "px", 96.0f);
        if (image != NULL) {
            w = (int)image->width;
            h = (int)image->height;

            rast = nsvgCreateRasterizer();
            data.resize(w*h*4);
            nsvgRasterize(rast, image, 0,0,1, data.data(), w, h, w*4);

            nsvgDeleteRasterizer(rast);
            nsvgDelete(image);
        }

        D3D12_RESOURCE_DESC desc = {
            D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
            D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
            (UINT64)w, //UINT64 Width;
            (UINT)h, //UINT Height;
            1, //UINT16 DepthOrArraySize;
            1, //UINT16 MipLevels;
            DXGI_FORMAT_R8G8B8A8_UNORM, //DXGI_FORMAT Format;
            DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
            D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
            D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //D3D12_RESOURCE_FLAGS Flags;
        };
        // Describe and create a SRV for the texture.
        D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
        shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shader_resource_view_desc.Format = desc.Format;
        shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        shader_resource_view_desc.Texture2D.MipLevels = 1;

        _shader_resource = _draw_system->MakeShaderResource(
            command_list->GetCommandList(), //ID3D12GraphicsCommandList* const in_command_list,
            _draw_system->MakeHeapWrapperCbvSrvUav(), //const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
            desc, //const D3D12_RESOURCE_DESC & in_desc,
            shader_resource_view_desc, //const D3D12_SHADER_RESOURCE_VIEW_DESC & in_shader_resource_view_desc,
             data //const std::vector < uint8_t >&in_data
            );

        auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "shader" / "screen_quad_vertex.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._root_path / "shader" / "screen_quad_pixel_texture.cso");
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
        array_shader_resource_info.push_back(ShaderResourceInfo::FactoryDataSampler(
            _shader_resource->GetHeapWrapperItem(),
            D3D12_SHADER_VISIBILITY_PIXEL
            ));

        _shader = _draw_system->MakeShader(
            command_list->GetCommandList(),
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info
            );
    }

    {
        const auto vertex_data = VectorHelper::FactoryArrayLiteral({
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            });
        _geometry = _draw_system->MakeGeometryGeneric(
            command_list->GetCommandList(),
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            input_element_desc_array,
            vertex_data,
            2
            );
    }
}

ApplicationTestSvg::~ApplicationTestSvg()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _shader_resource.reset();
    _shader.reset();
    _geometry.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationTestSvg  dtor %p",
        this
        );
}

void ApplicationTestSvg::Update()
{
    BaseType::Update();
    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());
        _shader->SetShaderResourceViewHandle(0, _shader_resource->GetHeapWrapperItem());
        frame->SetShader(_shader);
        frame->Draw(_geometry);
    }
}

void ApplicationTestSvg::OnWindowSizeChanged(
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

