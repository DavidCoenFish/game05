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
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_data/i_ui_data.h"
#include "common/text/text_manager.h"
#include "common/util/vector_helper.h"

UIContentFactoryParam::UIContentFactoryParam(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    TextManager* const in_text_manager
    )
    : _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _text_manager(in_text_manager)
{
    // Nop
}

UIManagerUpdateParam::UIManagerUpdateParam(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const IUIModel* const in_ui_model,
    const UIDataTextRunStyle* const in_default_text_style,
    LocaleSystem* const in_locale_system,
    TextManager* const in_text_manager,
    const float in_ui_scale,
    const float in_time_delta,
    const bool in_draw_every_frame, // mark top level as dirty each frame, the destination render target may have other systems drawing to it
    const bool in_draw_to_texture, // Draw to texture or backbuffer?
    const VectorInt2& in_texture_size, // If in_draw_to_texture is true, size to use for texture
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
    : _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_model(in_ui_model)
    , _locale_system(in_locale_system)
    , _text_manager(in_text_manager)
    , _ui_scale(in_ui_scale)
    , _time_delta(in_time_delta)
    , _draw_to_texture(in_draw_to_texture)
    , _draw_every_frame(in_draw_every_frame)
    , _texture_size(in_texture_size)
    , _allow_clear(in_allow_clear)
    , _clear_colour(in_clear_colour)
    , _default_text_style(in_default_text_style)
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

        // add a default content 
        //AddContentFactory("", FactoryDefault);

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
    void Update(
        std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
        const UIManagerUpdateParam& in_param,
        const std::string& in_model_key
        )
    {
        UIHierarchyNodeUpdateHierarchyParam hierarchy_param(
            _map_content_factory,
            in_param._draw_system,
            in_param._command_list,
            in_param._ui_model,
            in_param._locale_system,
            in_param._text_manager,
            in_param._default_text_style
            );
        if (false == in_param._ui_model->VisitDataArray(
            in_model_key,
            [&in_out_target_or_null, &in_param, &hierarchy_param](const std::vector<std::shared_ptr<IUIData>>& in_array_data){
                if (nullptr == in_out_target_or_null)
                {
                    in_out_target_or_null = std::make_shared<UIHierarchyNode>();
                }

                bool dirty = false;
                if(true == in_out_target_or_null->UpdateHierarchy(
                    hierarchy_param,
                    &in_array_data,
                    in_param._draw_to_texture,
                    in_param._draw_every_frame,
                    in_param._allow_clear,
                    in_param._clear_colour
                    ))
                {
                    dirty = true;
                }

                const VectorInt2 top_level_size = in_out_target_or_null->GetTextureSize(
                    in_param._draw_system
                    );

                in_out_target_or_null->UpdateSize(
                    in_param._draw_system,
                    top_level_size,
                    in_param._ui_scale,
                    in_param._time_delta,
                    dirty
                    );
            }))
        {
            in_out_target_or_null = nullptr;
        }
        return;
    }

    void DealInput(
        UIHierarchyNode& in_root,
        const UIManagerDealInputParam& in_param
        )
    {
        in_param; in_root;
        //DSC_TODO("deal input");
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
void UIManager::Update(
    std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
    const UIManagerUpdateParam& in_param,
    const std::string& in_model_key
    )
{
    _implementation->Update(
        in_out_target_or_null, 
        in_param,
        in_model_key
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
