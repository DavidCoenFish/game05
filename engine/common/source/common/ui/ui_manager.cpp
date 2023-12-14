#include "common/common_pch.h"
#include "common/ui/ui_manager.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_enum.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_root_input_state.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_component/ui_component_tooltip_layer.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_effect/ui_effect_component.h"
#include "common/util/vector_helper.h"

namespace
{
    static const char* const s_array_shader_path[static_cast<int>(UIShaderEnum::TCount)] = {
        "ui_default_pixel.cso",
        "ui_effect_debug_pixel.cso",
        "ui_effect_dropshadow_pixel.cso",
        "ui_effect_innershadow_pixel.cso",
        "ui_effect_corner_pixel.cso",
        "ui_effect_corner_hollow_pixel.cso",
        "ui_effect_gloss_pixel.cso",
        "ui_effect_fill_pixel.cso",
        "ui_effect_desaturate_pixel.cso"
        };
}

UIComponentFactoryParam::UIComponentFactoryParam(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    TextManager* const in_text_manager,
    const int in_create_index
    )
    : _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _text_manager(in_text_manager)
    , _create_index(in_create_index)
{
    // Nop
}

UIManagerUpdateParam::UIManagerUpdateParam(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const UIDataTextRunStyle* const in_default_text_style,
    LocaleSystem* const in_locale_system,
    TextManager* const in_text_manager,
    const float in_ui_scale,
    const float in_time_delta,
    const bool in_draw_every_frame, // mark top level as dirty each frame, the destination render target may have other systems drawing to it
    const bool in_draw_to_texture, // Draw to texture or backbuffer?
    const VectorInt2& in_texture_size // If in_draw_to_texture is true, size to use for texture
    )
    : _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _locale_system(in_locale_system)
    , _text_manager(in_text_manager)
    , _ui_scale(in_ui_scale)
    , _time_delta(in_time_delta)
    , _draw_to_texture(in_draw_to_texture)
    , _draw_every_frame(in_draw_every_frame)
    , _texture_size(in_texture_size)
    , _default_text_style(in_default_text_style)
{
    // Nop
}

UIManagerDealInputTouch::UIManagerDealInputTouch(
    const VectorInt2& in_root_relative_pos,
    const bool in_active,
    const int in_id,
    const UITouchFlavour in_flavour
    )
    : _root_relative_pos(in_root_relative_pos)
    , _active(in_active)
    , _id(in_id)
    , _flavour(in_flavour)
{
    // Nop
}

UIManagerDealInputNavigation::UIManagerDealInputNavigation(
    const UINavigationType in_type,
    const float in_amount,
    const bool in_end
    )
    : _type(in_type)
    , _amount(in_amount)
    , _end(in_end)
{
    // Nop
}

UIManagerDealInputParam::UIManagerDealInputParam(
    const std::vector<UIManagerDealInputTouch>& in_touch_array,
    const std::vector<UIManagerDealInputNavigation>& in_navigation_array,
    DrawSystem* const in_draw_system,
    const float in_time_delta
    )
    : _touch_array(in_touch_array)
    , _navigation_array(in_navigation_array)
    , _draw_system(in_draw_system)
    , _time_delta(in_time_delta)
{
    // Nop
}

UIManagerDrawParam::UIManagerDrawParam(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    TextManager* const in_text_manager,
    UIManager* const in_ui_manager,
    const float in_ui_scale
    )
    : _draw_system(in_draw_system)
    , _frame(in_frame)
    , _text_manager(in_text_manager)
    , _ui_manager(in_ui_manager)
    , _ui_scale(in_ui_scale)
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
        std::vector < DXGI_FORMAT > render_target_format;
        render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
        ShaderPipelineStateData shader_pipeline_state_data(
            UIGeometry::GetInputElementDescArray(),
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN,
            // DXGI_FORMAT_D32_FLOAT,
            render_target_format,
            ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied(),  //CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
            );

        auto vertex_shader_data = FileSystem::SyncReadFile(in_root_path / "shader" / "ui_vertex.cso");

        std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
        array_shader_resource_info.push_back(
            ShaderResourceInfo::FactoryClampSampler(nullptr, D3D12_SHADER_VISIBILITY_PIXEL)
            );
        std::vector<std::shared_ptr<ConstantBufferInfo>> array_shader_constants_info_default;
        array_shader_constants_info_default.push_back(
            ConstantBufferInfo::Factory(
                UIManager::TShaderConstantBuffer(),
                D3D12_SHADER_VISIBILITY_PIXEL
                )
            );
        std::vector<std::shared_ptr<ConstantBufferInfo>> array_shader_constants_info_effect;
        array_shader_constants_info_effect.push_back(
            ConstantBufferInfo::Factory(
                UIEffectComponent::TShaderConstantBuffer(),
                D3D12_SHADER_VISIBILITY_PIXEL
                )
            );

        for (int index = 0; index < static_cast<int>(UIShaderEnum::TCount); ++index)
        {
            const std::filesystem::path path = in_root_path / "shader" / std::string(s_array_shader_path[index]);
            auto pixel_shader_data = FileSystem::SyncReadFile( path );

            _shader_array[index] = in_draw_system->MakeShader(
                in_command_list,
                shader_pipeline_state_data,
                vertex_shader_data,
                nullptr,
                pixel_shader_data,
                array_shader_resource_info,
                0 != index ? array_shader_constants_info_effect : array_shader_constants_info_default
                );
        }

        {
            std::vector<uint8_t> vertex_data_raw;
            UIGeometry::BuildGeometryData(
                vertex_data_raw,
                VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
                VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
                );

            _effect_geometry = in_draw_system->MakeGeometryGeneric(
                in_command_list,
                D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
                UIGeometry::GetInputElementDescArray(),
                vertex_data_raw,
                4
                );
        }

        return;
    }

    // Update layout
    void Update(
        UIManager* const in_manager,
        std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data,
        const UIManagerUpdateParam& in_param,
        const bool in_render_to_texture,
        const VectorInt2& in_render_texture_size,
        const bool in_always_dirty,
        const bool in_allow_clear,
        const VectorFloat4& in_clear_colour
        )
    {
        if (0 == in_array_child_data.size())
        {
            if (nullptr != in_out_target_or_null)
            {
                in_out_target_or_null = nullptr;
            }
        }
        else
        {
            bool dirty = false;
            if (nullptr == in_out_target_or_null)
            {
                in_out_target_or_null = std::make_shared<UIHierarchyNode>();
                dirty = true;
            }

            UIHierarchyNodeUpdateParam update_param(
                in_param._draw_system,
                in_param._command_list,
                in_manager,
                in_param._locale_system,
                in_param._text_manager,
                in_param._default_text_style,
                in_param._ui_scale,
                in_param._time_delta
                );
            in_out_target_or_null->UpdateHierarchy(
                update_param, 
                in_array_child_data,
                std::vector<std::shared_ptr<UIEffectData>>(),
                0,
                dirty,
                in_render_to_texture,
                in_always_dirty,
                in_allow_clear,
                in_clear_colour
                );
            if (true == in_render_to_texture)
            {
                in_out_target_or_null->SetTextureSize(
                    in_render_texture_size
                    );
            }

            const VectorInt2 target_size = in_out_target_or_null->GetTextureSize(in_param._draw_system);
            in_out_target_or_null->UpdateLayout(
                update_param,
                target_size,
                VectorInt2::s_zero
                );

            in_out_target_or_null->UpdateResources(
                update_param,
                UIScreenSpace()
                );
        }

        return;
    }

    void DealInput(
        UIHierarchyNode& in_root,
        UIData* const in_tooltip_layer,
        UIData* const in_combo_box_layer,
        const UIManagerDealInputParam& in_param
        )
    {
        const VectorInt2 texture_size = in_root.GetTextureSize(in_param._draw_system);
        UIRootInputState& input_state = in_root.GetOrMakeRootInputState();
        input_state.Update(in_param, texture_size, in_tooltip_layer, in_combo_box_layer);

        in_root.DealInput(
            input_state,
            true 
            );

        input_state.FinialiseTooltip();
        input_state.FinialiseComboBox();

        //LOG_MESSAGE_DEBUG("%d %d %d %d", in_param._mouse_x, in_param._mouse_y, in_param._mouse_left_down, in_param._mouse_right_down, in_param._mouse_scroll);
        //where is the mouse
    }

    void Draw(
        UIHierarchyNode& in_root,
        const UIManagerDrawParam& in_param
        )
    {
        const bool dirty = in_root.PreDraw(
            in_param
            );
        in_root.Draw(
            in_param,
            dirty,
            UIStateFlag::TNone
            );
    }

    const std::shared_ptr<Shader>& GetShaderRef(const UIShaderEnum in_type)
    {
        return _shader_array[static_cast<size_t>(in_type)];
    }

    std::shared_ptr<GeometryGeneric>& GetEffectGeometryRef()
    {
        return _effect_geometry;
    }

private:
    std::shared_ptr<Shader> _shader;
    std::array<std::shared_ptr<Shader>, static_cast<size_t>(UIEffectEnum::TCount)> _shader_array;

    std::shared_ptr<GeometryGeneric> _effect_geometry;

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

// Get the InputElementDescArray to match the ui manager shader, not UIGeometry.h is not public
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

// Update layout
void UIManager::Update(
    std::shared_ptr<UIHierarchyNode>& in_out_target_or_null,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data,
    const UIManagerUpdateParam& in_param,

    const bool in_render_to_texture, // if false, rather than render to texture, we render to the draw system backbuffer
    const VectorInt2& in_render_texture_size, // if in_render_to_texture is true, 
    const bool in_always_dirty, //still draw to the top level target even if nothing has changed
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
{
    _implementation->Update(
        this,
        in_out_target_or_null, 
        in_array_child_data,
        in_param,
        in_render_to_texture, // if false, rather than render to texture, we render to the draw system backbuffer
        in_render_texture_size, // if in_render_to_texture is true, 
        in_always_dirty, //still draw to the top level target even if nothing has changed
        in_allow_clear,
        in_clear_colour
        );
    return;
}

// Deal input
void UIManager::DealInput(
    UIHierarchyNode& in_root,
    UIData* const in_tooltip_layer,
    UIData* const in_combo_box_layer,
    const UIManagerDealInputParam& in_param
    )
{
    _implementation->DealInput(
        in_root, 
        in_tooltip_layer,
        in_combo_box_layer,
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

const std::shared_ptr<Shader>& UIManager::GetShaderRef(const UIShaderEnum in_type) const
{
    return _implementation->GetShaderRef(in_type);
}

std::shared_ptr<GeometryGeneric>& UIManager::GetEffectGeometryRef()
{
    return _implementation->GetEffectGeometryRef();
}
