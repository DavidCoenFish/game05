#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_disable.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_component/ui_component_effect.h"
#include "common/ui/ui_data/ui_data_disable.h"

UIComponentDisable::UIComponentDisable(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const bool in_disable
    )
    : _component_default(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    //, _disable(false)
{
     _component_default.SetStateFlagDirty(UIStateFlag::TDisable);
     Set(in_disable);
}

UIComponentDisable::~UIComponentDisable()
{
    // Nop
}

const bool UIComponentDisable::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
{
    return _component_default.SetBase(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        );
}

const bool UIComponentDisable::Set(
    const bool in_disable
    )
{
    bool dirty = false;

    if (true == _component_default.SetStateFlagBit(UIStateFlag::TDisable, in_disable))
    {
        dirty = true;
    }

    return dirty;
}

const bool UIComponentDisable::SetStateFlag(const UIStateFlag in_state_flag)
{
    return _component_default.SetStateFlag(in_state_flag);
}

const bool UIComponentDisable::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentDisable::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentDisable::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentDisable::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
{
    _component_default.SetUVScrollManual(in_uv_scroll, in_manual_horizontal, in_manual_vertical);
    return;
}

void UIComponentDisable::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

// Make sorting children easier
void UIComponentDisable::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentDisable::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentDisable::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;

    const UIDataDisable* const data_disable = dynamic_cast<const UIDataDisable*>(in_data);
    if (nullptr != data_disable)
    {
        if (true == Set(data_disable->GetDisabled()))
        {
            dirty = true;
        }
    }

    if (true == _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        ))
    {
        dirty = true;
    }

    if (nullptr == _shader_constant_buffer)
    {
        const Shader* const shader = in_param._ui_manager->GetShaderRef(UIShaderEnum::TEffectDesaturate).get();
        if (nullptr != shader)
        {
            _shader_constant_buffer = shader->MakeShaderConstantBuffer(
                in_param._draw_system
                );
        }
    }

    return dirty;
}

const bool UIComponentDisable::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space
    )
{
    bool dirty = _component_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta, 
        in_out_geometry, 
        in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
        in_parent_screen_space,
        out_screen_space
        );

    if (nullptr != _shader_constant_buffer)
    {
        const VectorInt2 texture_size = in_out_node.GetTextureSize(in_draw_system);
        UIManager::TShaderConstantBuffer& constant_0 = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
        UIComponentEffect::TShaderConstantBuffer& constant_1 = _shader_constant_buffer->GetConstant<UIComponentEffect::TShaderConstantBuffer>(1);
        constant_0._tint_colour = _component_default.GetTintColour();

        const bool disable = (0 != (static_cast<int>(_component_default.GetStateFlag()) & static_cast<int>(UIStateFlag::TDisable)));

        VectorFloat2 texture_size_float(static_cast<float>(texture_size.GetX()), static_cast<float>(texture_size.GetY()));
        constant_1._data = VectorFloat4(
            disable ? 1.0f : 0.0f
            );
        constant_1._width_height_iwidth_iheight = VectorFloat4(
            static_cast<float>(texture_size.GetX()),
            static_cast<float>(texture_size.GetY()),
            1.0f / static_cast<float>(texture_size.GetX()),
            1.0f / static_cast<float>(texture_size.GetY())
            );
    }

    return dirty;
}

void UIComponentDisable::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
#if 1
    VectorInt2 max_desired_size;

    out_layout_size = _component_default.GetInUseLayout().GetSize(in_parent_window, in_ui_scale);
    
    // Default is to go through children and see if anyone needs a bigger size than what we calculate
    for (auto iter: in_out_node.GetChildData())
    {
        UIHierarchyNodeChildData& child_data = *iter;
        if (nullptr == child_data._component)
        {
            continue;
        }
        VectorInt2 child_layout_size;
        VectorInt2 child_desired_size;
        child_data._component->GetDesiredSize(
            child_layout_size,
            child_desired_size,
            out_layout_size, 
            in_ui_scale, 
            *child_data._node
            );
        max_desired_size = VectorInt2::Max(max_desired_size, child_desired_size);
    }

    out_layout_size = _component_default.GetInUseLayout().CalculateShrinkSize(out_layout_size, max_desired_size);
    out_desired_size = VectorInt2::Max(out_layout_size, max_desired_size);

    return;
#else
    return _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );
#endif
}

const bool UIComponentDisable::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    )
{
    bool dirty = false;

    if (true == in_node.PreDraw(
        in_draw_param
        ))
    {
        dirty = true;
    }

    UITexture& texture = in_node.GetUITexture();
    if ((true == dirty) ||
        (false == texture.GetHasDrawn()) ||
        (true == texture.GetAlwaysDirty())
        )
    {
        if (false == texture.SetRenderTarget(
            in_draw_param._draw_system,
            in_draw_param._frame
            ))
        {
            return dirty;
        }

        dirty = true;

        auto& child_data_array = in_node.GetChildData();
        if (0 != child_data_array.size())
        {
            UIHierarchyNodeChildData& child_data = *(child_data_array[0]);
            const auto& shader = in_draw_param._ui_manager->GetShaderRef(UIShaderEnum::TEffectDesaturate);

            child_data._node->GetUITexture().SetShaderResource(
                *shader,
                0,
                in_draw_param._frame
                );

            // the tint array index may be changed by input after update size and before draw
            if (nullptr != _shader_constant_buffer)
            {
                UIManager::TShaderConstantBuffer& constant_0 = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
                const auto new_tint = _component_default.GetTintColour();
                if (constant_0._tint_colour != new_tint)
                {
                    constant_0._tint_colour = new_tint;
                }
            }

            in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

            VectorFloat4 geometry_pos;
            VectorFloat4 geometry_uv;
            child_data._geometry->Get(
                geometry_pos,
                geometry_uv
                );
            DSC_ASSERT(geometry_pos == VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f), "Expect child geometry to be full screen");
            // atention Y inverted
            DSC_ASSERT(geometry_uv == VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f), "Expect child geometry to be full screen uv"); 

            child_data._geometry->Draw(
                in_draw_param._draw_system,
                in_draw_param._frame
                );
        }

        texture.SetHasDrawn(true);
    }

    return dirty;
}

// this Component already uses the tint colour in it's custom PreDraw
const VectorFloat4 UIComponentDisable::GetTintColour() const
{
    return VectorFloat4::s_white;
}

