#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node_child.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/ui/i_ui_input.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_effect/ui_effect_component.h"
#include "common/ui/ui_effect/ui_effect_data.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_root_input_state.h"

std::shared_ptr<UIHierarchyNodeChild> UIHierarchyNodeChild::Factory(
    UIHierarchyNodeChild* const in_parent_to_this_node_or_null,
    const UILayout& in_layout,
    const UITintColour& in_tint_colour,
    void* in_source_token,
    const UIStateFlag in_state_flag
    )
{
    auto geometry = std::make_unique<UIGeometry>();
    std::unique_ptr<IUIComponent> component;
    auto node = std::make_unique<UIHierarchyNode>();
    auto screen_space = std::make_unique<UIScreenSpace>();
    return std::make_shared<UIHierarchyNodeChild>(
        in_parent_to_this_node_or_null,
        geometry,
        component,
        node,
        screen_space,
        in_layout,
        in_tint_colour,
        in_source_token,
        in_state_flag
        );
}

UIHierarchyNodeChild::UIHierarchyNodeChild(
    UIHierarchyNodeChild* const in_parent_or_null,
    std::unique_ptr<UIGeometry>& in_geometry,
    std::unique_ptr<IUIComponent>& in_component,
    std::unique_ptr<UIHierarchyNode>& in_node,
    std::unique_ptr<UIScreenSpace>& in_screen_space,
    const UILayout& in_layout,
    const UITintColour& in_tint_colour,
    void* in_source_token,
    const UIStateFlag in_state_flag
    )
    : _parent_or_null(in_parent_or_null)
    , _geometry(std::move(in_geometry))
    , _component(std::move(in_component))
    , _node(std::move(in_node))
    , _screen_space(std::move(in_screen_space))
    , _layout(in_layout)
    , _tint_colour(in_tint_colour)
    , _source_token(in_source_token)
    , _state_flag(in_state_flag)
{
    // Nop
}

#if 0
void UIHierarchyNodeChild::Draw(const UIManagerDrawParam& in_draw_param)
{
    const auto& shader = in_draw_param._ui_manager->GetShaderRef(UIShaderEnum::TDefault);

    _node->GetUITexture().SetShaderResource(
        *shader,
        0,
        in_draw_param._frame
        );

    if (nullptr == _shader_constant_buffer)
    {
        _shader_constant_buffer = shader->MakeShaderConstantBuffer(
            in_draw_param._draw_system
            );
    }

    if (nullptr != _shader_constant_buffer)
    {
        UIManager::TShaderConstantBuffer& buffer = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
        buffer._tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

    _geometry->Draw(
        in_draw_param._draw_system,
        in_draw_param._frame
        );

    return;
}

const bool UIHierarchyNodeChild::VisitComponents(const std::function<const bool(IUIComponent* const, UIHierarchyNode* const)>& in_visitor)
{
    bool keep_going = true;
    auto* component = _component.get();
    if (nullptr != component)
    {
        keep_going = in_visitor(component, _node.get());
    }
    if (true == keep_going)
    {
        for (auto iter : _node->GetChildData())
        {
            keep_going = iter->VisitComponents(in_visitor);
            if (false == keep_going)
            {
                break;
            }
        }
    }

    return keep_going;
}
#endif


void UIHierarchyNodeChild::ApplyComponent(
    UIData& in_data,
    const UIHierarchyNodeUpdateParam& in_param,
    const int in_child_index
    )
{
    if (_layout != in_data.GetLayout())
    {
        _layout = in_data.GetLayout();
        SetStateFlagBit(UIStateFlag::TLayoutDirty, true);
    }

    if (_tint_colour != in_data.GetTintColour())
    {
        _tint_colour = in_data.GetTintColour();
        SetStateFlagBit(UIStateFlag::TRenderDirty, true);
    }

    if (true == in_data.ApplyComponent(
        _component,
        in_param,
        in_child_index
        ))
    {
        SetStateFlagBit(UIStateFlag::TRenderDirty, true);
    }
}

void UIHierarchyNodeChild::UpdateHierarchy(
    UIData& in_data,
    const UIHierarchyNodeUpdateParam& in_param,
    const int in_child_index
    )
{
    DSC_ASSERT(nullptr != _node, "node should be passed into ctor, what happened");
    _node->UpdateHierarchy(
        in_data,
        this,
        in_param,
        in_child_index
        );
}

const VectorInt2 UIHierarchyNodeChild::GetDesiredSize(
    const UIHierarchyNodeUpdateParam& in_layout_param,
    const VectorInt2& in_parent_window
    )
{
    if (nullptr != _component)
    {
        return _component->GetDesiredSize(
            *this,
            in_layout_param,
            in_parent_window
            );
    }
    return VectorInt2::s_zero;
}

UIHierarchyNode& UIHierarchyNodeChild::GetNode() const
{
    DSC_ASSERT(nullptr != _node, "node should be passed into ctor, what happened");
    return *_node;
}

UIScreenSpace& UIHierarchyNodeChild::GetScreenSpace() const
{
    DSC_ASSERT(nullptr != _screen_space, "screen space should be passed into ctor, what happened");
    return *_screen_space;
}

void UIHierarchyNodeChild::Finalise(
    const VectorInt2& in_base_layout_size,
    const VectorInt2& in_base_desired_size,
    const VectorInt2& in_parent_offset
    )
{
    VectorInt2 texture_size;

    // finialise layout size (shrink/expand) and work out the texture size (which may include the texture margin)
    _layout.Finalise(
        _layout_size,
        texture_size,
        _layout_offset,
        in_base_layout_size,
        in_base_desired_size,
        in_parent_offset
        );

    _node->SetTextureSize(texture_size);
}

void UIHierarchyNodeChild::UpdateLayout(
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset
    )
{
    _component->UpdateLayout(
        *this,
        in_param,
        in_parent_window,
        in_parent_offset
        );
}

void UIHierarchyNodeChild::Update(
    const float in_time_delta
    )
{
    const float prev_time = _time_accumulate_seconds;
    _time_accumulate_seconds += in_time_delta;
    const bool dirty = _tint_colour.GetTimeChangeDirty(prev_time, _time_accumulate_seconds);
    if (true == dirty)
    {
        SetStateFlagBit(UIStateFlag::TRenderDirty, true);
    }
    return;
}

void UIHierarchyNodeChild::UpdateScroll(
    const UIHierarchyNodeUpdateParam& in_param
    )
{
    const bool uv_scroll_manual_x = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollX));
    const bool uv_scroll_manual_y = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollY));

    const VectorInt2 texture_size = _node->GetTextureSize(in_param._draw_system);

    IUIComponent::UpdateScroll(
        _uv_scroll,
        uv_scroll_manual_x,
        uv_scroll_manual_y,
        in_param._delta_time_seconds,
        _layout_size,
        texture_size
        );

    return;
}

void UIHierarchyNodeChild::UpdateGeometry(
    const UIHierarchyNodeUpdateParam& in_param,
    const UIScreenSpace& in_parent_screen_space,
    const VectorInt2& in_parent_texture_size
    )
{
    const VectorInt2 texture_size = _node->GetTextureSize(in_param._draw_system);

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    IUIComponent::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        in_parent_texture_size,
        _layout_offset,
        _layout_size,
        texture_size,
        _uv_scroll,
        _layout
        );

    if (true == _geometry->Set(
        geometry_pos,
        geometry_uv
        ))
    {
        SetStateFlagBit(UIStateFlag::TRenderDirty, true);
    }

    _screen_space->Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    return;
}

/*
const bool UIHierarchyNodeChild::RecurseSetStateFlagInput(UIHierarchyNodeChild* const in_data, const UIStateFlag in_state_flag)
{
    bool dirty = false;

    if (nullptr == in_data)
    {
        return dirty;
    }

    IUIComponent* const component = in_data->_component.get();
    if (nullptr != component)
    {
        int state_flag = static_cast<int>(component->GetStateFlag());
        state_flag &= ~static_cast<int>(UIStateFlag::TMaskInput);
        state_flag |= (static_cast<int>(in_state_flag) & static_cast<int>(UIStateFlag::TMaskInput));
        if (true == component->SetStateFlag(static_cast<UIStateFlag>(state_flag)))
        {
            dirty = true;
        }
    }

    UIHierarchyNode* const node = in_data->_node.get();
    if (nullptr != node)
    {
        for (auto iter : node->GetChildData())
        {
            if (true == RecurseSetStateFlagInput(iter.get(), in_state_flag))
            {
                dirty = true;
            }
        }
    }

    return dirty;
}
*/
