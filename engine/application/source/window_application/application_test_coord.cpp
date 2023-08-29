#include "application_pch.h"

#include "window_application/application_test_coord.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_content/ui_content_texture.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

IWindowApplication* const ApplicationTestCoord::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestCoord(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestCoord::ApplicationTestCoord(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _screen_size(0)
{
    LOG_MESSAGE(
        "ApplicationTestCoord  ctor %p",
        this
        );

    // DrawSystem
    {
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
            render_target_format_data
            );
    }

    _draw_resource = std::make_unique<DrawResource>();

    auto command_list = _draw_system->CreateCustomCommandList();

    // Render target
    {
        const std::vector<RenderTargetFormatData> target_format_data_array({
            RenderTargetFormatData(
                DXGI_FORMAT_B8G8R8A8_UNORM,
                true,
                VectorFloat4(0.0f, 1.0f, 0.0f, 1.0f)
                )
            });

        _draw_resource->_render_target = _draw_system->MakeRenderTargetTexture(
            command_list->GetCommandList(),
            target_format_data_array,
            RenderTargetDepthData(),
            VectorInt2(256, 256)
            );
    }

    // Triangle shader and geometry
    {
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

        {
            auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "triangle_vertex.cso");
            auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "triangle_pixel.cso");
            std::vector < DXGI_FORMAT > render_target_format;
            render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
            ShaderPipelineStateData shader_pipeline_state_data(
                input_element_desc_array,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                DXGI_FORMAT_UNKNOWN,
                render_target_format,
                CD3DX12_BLEND_DESC(D3D12_DEFAULT),
                CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                CD3DX12_DEPTH_STENCIL_DESC()
                );
            _draw_resource->_triangle_shader = _draw_system->MakeShader(
                command_list->GetCommandList(),
                shader_pipeline_state_data,
                vertex_shader_data,
                nullptr,
                pixel_shader_data
                );
        }

        {
            const auto vertex_data = VectorHelper::FactoryArrayLiteral({
                0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f
                });
            _draw_resource->_triangle_geometry = _draw_system->MakeGeometryGeneric(
                command_list->GetCommandList(),
                D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                input_element_desc_array,
                vertex_data,
                6
                );
        }
    }

    // Present shader and geometry
    {
        std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
        input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
            {
                "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
            });
        input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
            {
                "TEXCORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
            });

        {
            auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "present_vertex.cso");
            auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "present_pixel.cso");
            std::vector < DXGI_FORMAT > render_target_format;
            render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
            ShaderPipelineStateData shader_pipeline_state_data(
                input_element_desc_array,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                DXGI_FORMAT_UNKNOWN,
                render_target_format,
                CD3DX12_BLEND_DESC(D3D12_DEFAULT),
                CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                CD3DX12_DEPTH_STENCIL_DESC()
                );
            std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
            array_shader_resource_info.push_back(
                ShaderResourceInfo::FactorySampler(nullptr, D3D12_SHADER_VISIBILITY_PIXEL)
                );

            _draw_resource->_present_shader = _draw_system->MakeShader(
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
                0.5f, 0.5f, 0.0f, 1.0f,
                0.5f, 1.0f, 0.0f, 0.0f,
                1.0f, 0.5f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                });
            _draw_resource->_panel_geometry[0] = _draw_system->MakeGeometryGeneric(
                command_list->GetCommandList(),
                D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
                input_element_desc_array,
                vertex_data,
                4
            );
        }

        {
            const auto vertex_data = VectorHelper::FactoryArrayLiteral({
                0.5f, 0.0f, 0.0f, 1.0f,
                0.5f, 0.5f, 0.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.5f, 1.0f, 0.0f,

                });
            _draw_resource->_panel_geometry[1] = _draw_system->MakeGeometryGeneric(
                command_list->GetCommandList(),
                D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
                input_element_desc_array,
                vertex_data,
                4
            );
        }

    }

    // Texture
    {
        D3D12_RESOURCE_DESC desc = {
            D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
            D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
            256, //UINT64 Width;
            256, //UINT Height;
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

        std::vector<uint8_t> data;
        for(int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                data.push_back((uint8_t)x);
                data.push_back((uint8_t)y);
                data.push_back(0);
                data.push_back(255);
            }
        }

        _draw_resource->_texture = _draw_system->MakeShaderResource(
            command_list->GetCommandList(),
            _draw_system->MakeHeapWrapperCbvSrvUav(), //const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
            desc, //const D3D12_RESOURCE_DESC & in_desc,
            shader_resource_view_desc, //const D3D12_SHADER_RESOURCE_VIEW_DESC & in_shader_resource_view_desc,
            data //const std::vector<uint8_t>&in_data
            );

    }

    // UI
#if 0
    _draw_resource->_ui_manager = std::make_unique<UIManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );
     {
        _draw_resource->_ui_hierarchy_node = std::make_shared<UIHierarchyNode>(
            UIHierarchyNode::LayoutData::FactoryFull(),
            UIHierarchyNode::MakeContentCanvas(),
            UIHierarchyNode::MakeTextureBackBuffer(
                _draw_system.get(),
                command_list->GetCommandList(),
                false, // Allow_clear
                true // Always dirty
                )
            );

        {
            _draw_resource->_ui_texture[0] = std::dynamic_pointer_cast<UIContentTexture>(UIHierarchyNode::MakeContentTexture(
                _draw_system.get(),
                command_list->GetCommandList(),
                _draw_resource->_render_target->GetShaderResourceHeapWrapperItem()
                ));
            auto node = std::make_shared<UIHierarchyNode>(
                UIHierarchyNode::LayoutData::LayoutData(
                    UICoord(0.25f, 0.0f),
                    UICoord(0.25f, 0.0f),
                    UICoord(0.75f, 0.0f),
                    UICoord(0.25f, 0.0f),
                    UICoord(0.0f, 0.0f),
                    UICoord(0.0f, 0.0f)
                    ),
                _draw_resource->_ui_texture[0],
                UIHierarchyNode::MakeTextureRenderTarget(
                    _draw_system.get(),
                    command_list->GetCommandList()
                    )
                );

            _draw_resource->_ui_hierarchy_node->AddChild(
                node,
                _draw_system.get(),
                command_list->GetCommandList()
                );
        }

        {
            _draw_resource->_ui_texture[1] = std::dynamic_pointer_cast<UIContentTexture>(UIHierarchyNode::MakeContentTexture(
                _draw_system.get(),
                command_list->GetCommandList(),
                _draw_resource->_texture->GetHeapWrapperItem()
                ));
            auto node = std::make_shared<UIHierarchyNode>(
                UIHierarchyNode::LayoutData::LayoutData(
                    UICoord(0.25f, 0.0f),
                    UICoord(0.25f, 0.0f),
                    UICoord(0.75f, 0.0f),
                    UICoord(0.0f, 0.0f),
                    UICoord(0.0f, 0.0f),
                    UICoord(0.0f, 0.0f)
                    ),
                _draw_resource->_ui_texture[1],
                UIHierarchyNode::MakeTextureRenderTarget(
                    _draw_system.get(),
                    command_list->GetCommandList()
                    )
                );

            _draw_resource->_ui_hierarchy_node->AddChild(
                node,
                _draw_system.get(),
                command_list->GetCommandList()
                );
        }
     }
#endif
}

ApplicationTestCoord::~ApplicationTestCoord()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _draw_resource.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationTestCoord  dtor %p",
        this
        );
}

void ApplicationTestCoord::Update()
{
    BaseType::Update();
    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        
        // Draw triangle to render texture
        frame->SetRenderTarget(_draw_resource->_render_target.get());
        frame->SetShader(_draw_resource->_triangle_shader.get());
        frame->Draw(_draw_resource->_triangle_geometry.get());
        frame->ResourceBarrier(_draw_resource->_render_target.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // Set back buffer as render target
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        // Draw triangle
        frame->SetShader(_draw_resource->_triangle_shader.get());
        frame->Draw(_draw_resource->_triangle_geometry.get());

        // Draw Panel 0 with render texture of triangle
        _draw_resource->_present_shader->SetShaderResourceViewHandle(0, _draw_resource->_render_target->GetShaderResourceHeapWrapperItem());
        frame->SetShader(_draw_resource->_present_shader.get());
        frame->Draw(_draw_resource->_panel_geometry[0].get());

        // Draw Panel 1 with texture
        _draw_resource->_present_shader->SetShaderResourceViewHandle(0, _draw_resource->_texture->GetHeapWrapperItem());
        frame->SetShader(_draw_resource->_present_shader.get());
        frame->Draw(_draw_resource->_panel_geometry[1].get());

        #if 0
        // Draw ui hierarchy
        //_draw_resource->_ui_texture[0]->SetTexture(_draw_resource->_render_target->GetShaderResourceHeapWrapperItem());
        //_draw_resource->_ui_texture[1]->SetTexture(_draw_resource->_texture->GetHeapWrapperItem());

        _draw_resource->_ui_manager->DrawHierarchy(
            _draw_system.get(),
            frame.get(),
            _draw_resource->_ui_hierarchy_node.get(),
            UIManagerDrawData(1.0f)
            );
        #endif
    }
}

void ApplicationTestCoord::OnWindowSizeChanged(
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

    _screen_size = VectorInt2(in_width, in_height);

    return;
}

