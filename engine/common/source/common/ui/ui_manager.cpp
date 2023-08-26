#include "common/common_pch.h"
#include "common/ui/ui_manager.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/file_system/file_system.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_data/i_ui_data.h"
#include "common/util/vector_helper.h"

UIContentFactoryParam::UIContentFactoryParam(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
    : _draw_system(in_draw_system)
    , _command_list(in_command_list)
{
    // Nop
}

UIManagerUpdateLayoutParam::UIManagerUpdateLayoutParam(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const IUIModel* const in_ui_model,
    LocaleSystem* const in_locale_system,
    TextManager* const in_text_manager,
    const float in_ui_scale,
    const float in_time_delta,
    const std::string& in_locale,
    const std::string& in_model_key,
    const bool in_draw_to_texture,
    const bool in_always_dirty,
    const VectorInt2& in_texture_size
    )
    : _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_model(in_ui_model)
    , _locale_system(in_locale_system)
    , _text_manager(in_text_manager)
    , _ui_scale(in_ui_scale)
    , _time_delta(in_time_delta)
    , _locale(in_locale)
    , _model_key(in_model_key)
    , _draw_to_texture(in_draw_to_texture)
    , _always_dirty(in_always_dirty)
    , _texture_size(in_texture_size)
{
    // Nop
}

UIManagerDrawParam::UIManagerDrawParam(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    TextManager* const in_text_manager
    )
    : _draw_system(in_draw_system)
    , _frame(in_frame)
    , _text_manager(in_text_manager)
{
    // Nop
}

class UIManagerImplementation
{
public:
    UIManagerImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        )
    {
        auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "ui_block_vertex.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "ui_block_pixel.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        ShaderPipelineStateData shader_pipeline_state_data(
            UIGeometry::GetInputElementDescArray(),
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN,
            // DXGI_FORMAT_D32_FLOAT,
            render_target_format,
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
            );

        std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
        array_shader_resource_info.push_back(
            ShaderResourceInfo::FactorySampler(nullptr, D3D12_SHADER_VISIBILITY_PIXEL)
            );

        _shader = in_draw_system->MakeShader(
            in_command_list,
            shader_pipeline_state_data,
            vertex_shader_data,
            nullptr,
            pixel_shader_data,
            array_shader_resource_info
            );
        return;
    }

    void AddContentFactory(
        const std::string& in_content_name,
        const UIManager::TContentFactory& in_factory
        )
    {
        _map_content_factory[in_content_name] = in_factory;
        return;
    }

    // Update layout
    void UpdateLayout(
        std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
        const UIManagerUpdateLayoutParam& in_param
        )
    {
        auto data = in_param._ui_model->GetData(in_param._model_key);
        if (nullptr == data)
        {
            in_out_target_or_null = nullptr;
            return;
        }

        //// This could be pushed down into node, but trying to avoid empty nodes for missing templates
        //auto found = _map_content_factory.find(data->GetTemplateName());
        //if (found == _map_content_factory.end())
        //{
        //    in_out_target_or_null = nullptr;
        //    return;
        //}

        if (nullptr == in_out_target_or_null)
        {
            in_out_target_or_null = std::make_shared<UIHierarchyNode>();
        }

        UIHierarchyNodeUpdateLayoutParam node_update_layout_param(
            in_param._locale,
            in_param._ui_scale,
            in_param._time_delta,
            _map_content_factory,
            in_param._draw_system,
            in_param._command_list,
            in_param._ui_model,
            in_param._locale_system
            );

        in_out_target_or_null->UpdateLayout(
            data,
            in_param._model_key,
            in_param._draw_to_texture,
            in_param._always_dirty,
            in_param._draw_to_texture ? 
                in_param._texture_size : 
                in_param._draw_system->GetRenderTargetBackBuffer()->GetSize(),
            node_update_layout_param
            );
    }

    void DealInput(
        UIHierarchyNode& in_root,
        const UIManagerDealInputParam& in_param
        )
    {
        in_param; in_root;
        DSC_TODO("deal input");
    }

    void Draw(
        UIHierarchyNode& in_root,
        const UIManagerDrawParam& in_param
        )
    {
        in_root.Draw(
            in_param,
            _shader.get()
            );
    }

private:
    std::shared_ptr<Shader> _shader;
    std::map<std::string, UIManager::TContentFactory> _map_content_factory;

};

UIManager::UIManager(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::filesystem::path& in_root_path
    )
{
    _implementation = std::make_unique<UIManagerImplementation>(
        in_draw_system, 
        in_command_list,
        in_root_path
        );
}

UIManager::~UIManager()
{
    // Nop
}

// Get the InputElementDescArray to match the ui manager shader
const std::vector<D3D12_INPUT_ELEMENT_DESC>& UIManager::GetInputElementDescArray()
{
    return UIGeometry::GetInputElementDescArray();
}

void UIManager::BuildGeometryData(
    std::vector<uint8_t>& out_vertex_data,
    // Left bottom, right top (pos, uv)
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
{
    UIGeometry::BuildGeometryData(
        out_vertex_data,
        in_pos,
        in_uv
        );
    return;
}

void UIManager::AddContentFactory(
    const std::string& in_content_name,
    const TContentFactory& in_factory
    )
{
    _implementation->AddContentFactory(
        in_content_name,
        in_factory
        );
    return;
}

// Update layout
void UIManager::UpdateLayout(
    std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
    const UIManagerUpdateLayoutParam& in_param
    )
{
    _implementation->UpdateLayout(
        in_out_target_or_null, 
        in_param
        );
    return;
}

// Deal input
void UIManager::DealInput(
    UIHierarchyNode& in_root,
    const UIManagerDealInputParam& in_param
    )
{
    _implementation->DealInput(
        in_root, 
        in_param
        );
    return;
}

// Draw
void UIManager::Draw(
    UIHierarchyNode& in_root,
    const UIManagerDrawParam& in_param
    )
{
    _implementation->Draw(
        in_root, 
        in_param
        );
    return;
}







#if 0

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/heap_wrapper/heap_wrapper.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/file_system/file_system.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_data/i_ui_provider_data.h"
#include "common/util/vector_helper.h"

namespace
{
    static const std::vector<D3D12_INPUT_ELEMENT_DESC> s_input_element_desc_array({
        D3D12_INPUT_ELEMENT_DESC
        {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        },
        D3D12_INPUT_ELEMENT_DESC
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
        }
    });
}

UIManagerDrawData::UIManagerDrawData(
    const float in_ui_scale,
    const std::string& in_locale,
    const float in_time_delta,
    const IUIProviderData* const in_ui_provider_data,
    UIManager* const in_ui_manager
    )
    : _ui_scale(in_ui_scale)
    , _locale(in_locale)
    , _time_delta(in_time_delta)
    , _ui_provider_data(in_ui_provider_data)
    , _ui_manager(in_ui_manager)
{
    // Nop
}

class UIManagerImplementation
{
private:
    typedef std::function<std::shared_ptr<UIHierarchyNode>(
            UIManager* const,
            DrawSystem* const,
            ID3D12GraphicsCommandList* const,
            IUIProviderData* const,
            const std::string& // key_base
            )> TNodeFactory;

public:
    UIManagerImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::filesystem::path& in_root_path
        )
    {
        auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "ui_block_vertex.cso");
        auto pixel_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "ui_block_pixel.cso");
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        ShaderPipelineStateData shader_pipeline_state_data(
            s_input_element_desc_array,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN,
            // DXGI_FORMAT_D32_FLOAT,
            render_target_format,
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
            );

            std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
            array_shader_resource_info.push_back(
                ShaderResourceInfo::FactorySampler(nullptr, D3D12_SHADER_VISIBILITY_PIXEL)
                );

            _shader = in_draw_system->MakeShader(
                in_command_list,
                shader_pipeline_state_data,
                vertex_shader_data,
                nullptr,
                pixel_shader_data,
                array_shader_resource_info
                );
    }

    void DrawHelper(
        DrawSystemFrame* const in_frame,
        IRenderTarget* const in_render_target,
        GeometryGeneric* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_texture_handle
        )
    {
        in_frame->SetRenderTarget(in_render_target);
        _shader->SetShaderResourceViewHandle(0, in_texture_handle);
        in_frame->Draw(in_geometry);
        return;
    }

    void DrawHierarchy(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        UIHierarchyNode* const in_node,
        const UIManagerDrawData& in_data
        )
    {
        in_node->DrawHierarchyRoot(
            in_draw_system,
            in_frame,
            _shader.get(),
            in_data
            );

        return;
    }

    void AddHierarchyNodeFactory(
        const std::string& in_template_name,
        const TNodeFactory& in_factory
        )
    {
        _map_hierarchy_node_factory[in_template_name] = in_factory;
    }

    std::shared_ptr<UIHierarchyNode> MakeHierarchyNode(
        UIManager* const in_ui_manager,
        const std::string& in_template_name,
        const std::string& in_provider_data_key_base,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        IUIProviderData* const in_provider_data
        )
    {
        auto found = _map_hierarchy_node_factory.find(in_template_name);
        if (found != _map_hierarchy_node_factory.end())
        {
            return found->second(
                in_ui_manager,
                in_draw_system,
                in_command_list,
                in_provider_data,
                in_provider_data_key_base
                );
        }
        return nullptr;
    }

    void BuildPage(
        UIManager* const in_ui_manager,
        UIHierarchyNode* const in_parent,
        const std::string& in_page_name, 
        const std::string& in_provider_data_key_base,
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        IUIProviderData* const in_provider_data
        )
    {
        const auto data_array = in_provider_data->GetPageTemplateArray(
            in_page_name,
            in_provider_data_key_base
            );
        for (const auto& item : data_array)
        {
            in_parent->AddChild(
                MakeHierarchyNode(
                    in_ui_manager,
                    item._template_name,
                    item._data_provider_root,
                    in_draw_system,
                    in_command_list,
                    in_provider_data
                    ),
                in_draw_system,
                in_command_list
                );
        }
    }

private:
    std::shared_ptr<Shader> _shader;
    std::map<std::string, TNodeFactory> _map_hierarchy_node_factory;

};

const std::vector<D3D12_INPUT_ELEMENT_DESC>& UIManager::GetInputElementDesc()
{
    return s_input_element_desc_array;
}

UIManager::UIManager(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::filesystem::path& in_root_path
    )
{
    _implementation = std::make_unique<UIManagerImplementation>(
        in_draw_system, 
        in_command_list,
        in_root_path
        );
}

UIManager::~UIManager()
{
    // Nop
}

std::shared_ptr<GeometryGeneric> UIManager::GeometryHelper(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
{
    return UIGeometry::GeometryHelper(
        in_draw_system,
        in_command_list,
        in_pos,
        in_uv
        );
}

void UIManager::GeometryUpdateHelper(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    GeometryGeneric* const in_geometry,
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
{
    UIGeometry::GeometryUpdateHelper(
        in_draw_system,
        in_command_list,
        in_geometry,
        in_pos,
        in_uv
        );
    return;
}

void UIManager::DrawHelper(
    DrawSystemFrame* const in_frame,
    IRenderTarget* const in_render_target,
    GeometryGeneric* const in_geometry,
    const std::shared_ptr<HeapWrapperItem>& in_texture_handle
    )
{
    _implementation->DrawHelper(
        in_frame,
        in_render_target,
        in_geometry,
        in_texture_handle
        );
    return;
}

void UIManager::DrawHierarchy(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    UIHierarchyNode* const in_node,
    const UIManagerDrawData& in_data
    )
{
    _implementation->DrawHierarchy(
        in_draw_system,
        in_frame,
        in_node,
        in_data
        );
    return;
}

void UIManager::AddHierarchyNodeFactory(
    const std::string& in_template_name,
    const std::function< std::shared_ptr<UIHierarchyNode>(
        UIManager* const,
        DrawSystem* const,
        ID3D12GraphicsCommandList* const,
        IUIProviderData* const,
        const std::string& // key_base
        )>& in_factory
    )
{
    _implementation->AddHierarchyNodeFactory(in_template_name, in_factory);
}

std::shared_ptr<UIHierarchyNode> UIManager::MakeHierarchyNode(
    const std::string& in_template_name,
    const std::string& in_provider_data_key_base,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    IUIProviderData* const in_provider_data
    )
{
    return _implementation->MakeHierarchyNode(
        this,
        in_template_name,
        in_provider_data_key_base,
        in_draw_system,
        in_command_list,
        in_provider_data
        );
}


void UIManager::BuildPage(
    UIHierarchyNode* const in_parent,
    const std::string& in_template_name, 
    const std::string& in_provider_data_key_base,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    IUIProviderData* const in_provider_data
    )
{
    _implementation->BuildPage(
        this,
        in_parent,
        in_template_name, 
        in_provider_data_key_base,
        in_draw_system,
        in_command_list,
        in_provider_data
        );
    return;
}
#endif