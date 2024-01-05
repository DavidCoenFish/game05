#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node_child.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/log/log.h"
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
    const UIStateFlag in_state_flag,
    const UIStateDirty in_state_dirty
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
        in_state_flag,
        in_state_dirty
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
    const UIStateFlag in_state_flag,
    const UIStateDirty in_state_dirty
    )
    : _parent_or_null(in_parent_or_null)
    , _geometry(std::move(in_geometry))
    , _component(std::move(in_component))
    , _component_input(nullptr)
    , _node(std::move(in_node))
    , _screen_space(std::move(in_screen_space))
    , _layout(in_layout)
    , _tint_colour(in_tint_colour)
    , _time_accumulate_seconds(0.0f)
    , _source_token(in_source_token)
    , _state_flag(in_state_flag)
    , _mark_render_dirty_on_state_flag_tint_mask_change(false)
    , _state_dirty(in_state_dirty)
{
    DSC_ASSERT(nullptr != _geometry, "geometry should be passed into ctor, what happened");
    DSC_ASSERT(nullptr != _node, "node should be passed into ctor, what happened");
    DSC_ASSERT(nullptr != _screen_space, "screen_space should be passed into ctor, what happened");

    _component_input = dynamic_cast<IUIInput*>(_component.get());

    return;
}

UIHierarchyNodeChild::~UIHierarchyNodeChild()
{
    // Nop
}

/// allows making a map externally of source token to IUIComponent
void* UIHierarchyNodeChild::GetSourceToken() const
{
    return _source_token;
}

void UIHierarchyNodeChild::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
{
    SetStateFlagBit(UIStateFlag::TManualScrollX, in_manual_horizontal);
    SetStateFlagBit(UIStateFlag::TManualScrollY, in_manual_vertical);

    if (true == in_manual_horizontal)
    {
        _uv_scroll[0] = in_uv_scroll[0];
    }
    if (true == in_manual_vertical)
    {
        _uv_scroll[1] = in_uv_scroll[1];
    }

    return;
}

void UIHierarchyNodeChild::SetStateFlag(const UIStateFlag in_state_flag)
{
    if (true == _mark_render_dirty_on_state_flag_tint_mask_change)
    {
        if ((static_cast<int>(in_state_flag) & static_cast<int>(UIStateFlag::TTintMask))
            !=
            (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TTintMask)))
        {
            SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
        }
    }
    _state_flag = in_state_flag;
}

void UIHierarchyNodeChild::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    int state_flag = static_cast<int>(_state_flag);
    if (true == in_enable)
    {
        state_flag |= static_cast<int>(in_state_flag_bit);
    }
    else
    {
        state_flag &= ~static_cast<int>(in_state_flag_bit);
    }

    #if 0
    if (_state_flag != state_flag)
    {
        LOG_CONSOLE("SetStateFlag %d => %d", _state_flag, in_state_flag);
    }
    #endif

    SetStateFlag(static_cast<UIStateFlag>(state_flag));

    return;
}

const bool UIHierarchyNodeChild::GetStateFlagBit(const UIStateFlag in_state_flag_bit) const
{
    return 0 != (static_cast<int>(_state_flag) & static_cast<int>(in_state_flag_bit));
}

void UIHierarchyNodeChild::SetStateDirtyBit(const UIStateDirty in_state_dirty_bit, const bool in_enable)
{
    int state_dirty = static_cast<int>(_state_dirty);
    if (true == in_enable)
    {
        state_dirty |= static_cast<int>(in_state_dirty_bit);
    }
    else
    {
        state_dirty &= ~static_cast<int>(in_state_dirty_bit);
    }

    _state_dirty = static_cast<UIStateDirty>(state_dirty);

    if ((true == in_enable) && (nullptr != _parent_or_null))
    {
        _parent_or_null->SetStateDirtyBit(in_state_dirty_bit, true);
    }

    return;
}

const bool UIHierarchyNodeChild::GetStateDirtyBit(const UIStateDirty in_state_dirty_bit) const
{
    return 0 != (static_cast<int>(_state_dirty) & static_cast<int>(in_state_dirty_bit));
}

/// Set layout dirty flag on change, 
/// WARNING, should SetLayout only be called from UIData, to move things like slider, set layout in UIData?
void UIHierarchyNodeChild::SetLayout(const UILayout& in_layout)
{
    if (_layout != in_layout)
    {
        _layout = in_layout;
        SetStateDirtyBit(UIStateDirty::TLayoutDirty, true);
    }
    return;
}

void UIHierarchyNodeChild::SetTintColour(const UITintColour& in_tint_colour)
{
    if (_tint_colour != in_tint_colour)
    {
        _tint_colour = in_tint_colour;
        SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
    }
    return;
}

const VectorFloat4 UIHierarchyNodeChild::CalculateTintColour() const
{
    return _tint_colour.GetTintColour(_time_accumulate_seconds);
}

void UIHierarchyNodeChild::ApplyComponent(
    UIData& in_data,
    const UIHierarchyNodeUpdateParam& in_param
    )
{
#ifdef _DEBUG
    _debug_name = in_data._debug_name;
#endif

    SetLayout(in_data.GetLayout());
    SetTintColour(in_data.GetTintColour());

    if (true == in_data.ApplyComponent(
        _component,
        in_param
        ))
    {
        _component_input = dynamic_cast<IUIInput*>(_component.get());

        // if the layout is dependent on 
        if (true == _layout.GetAdjustmentModifiesLayout())
        {
            SetStateDirtyBit(UIStateDirty::TLayoutDirty, true);
        }

        SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
    }
}

void UIHierarchyNodeChild::UpdateHierarchy(
    UIData& in_data,
    const UIHierarchyNodeUpdateParam& in_param
    )
{
    DSC_ASSERT(nullptr != _node, "node should be passed into ctor, what happened");
    _node->UpdateHierarchy(
        in_data,
        this,
        in_param
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
    const VectorInt2& in_base_desired_size//,
    //const VectorInt2& in_parent_window,
    //const VectorInt2& in_parent_offset
    )
{
    LOG_MESSAGE_UI_VERBOSE("  UIHierarchyNodeChild::Finalise %p %s", _source_token, _debug_name.c_str());

    VectorInt2 texture_size;

    // finialise layout size (shrink/expand) and work out the texture size (which may include the texture margin)
    _layout.Finalise(
        _layout_size,
        texture_size,
        _adjust_offset,
        in_base_layout_size,
        in_base_desired_size//,
        //in_parent_window,
        //in_parent_offset
        );

    DSC_ASSERT(nullptr != _node, "node should be passed into ctor, what happened");
    if (true == _node->SetTextureSize(texture_size))
    {
        SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
    }

    LOG_MESSAGE_UI_VERBOSE("      _texture_size:[%d,%d]", texture_size[0], texture_size[1]);

    return;
}

void UIHierarchyNodeChild::UpdateLayout(
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset
    )
{
    LOG_MESSAGE_UI_VERBOSE("  UIHierarchyNodeChild::UpdateLayout %p %s", _source_token, _debug_name.c_str());
    const bool parent_size_change = ((in_parent_window != _cache_parent_window) || (in_parent_offset != _cache_parent_offset));

    if ((true == parent_size_change) ||
        (true == GetStateDirtyBit(UIStateDirty::TLayoutDirty)))
    {
        SetStateDirtyBit(UIStateDirty::TLayoutDirty, false);

        // setting this above component::UpdateLayout, but better would be to use the param passed through rather than cached state
        _cache_parent_window = in_parent_window;
        _cache_parent_offset = in_parent_offset;

        if (nullptr != _component)
        {
            _component->UpdateLayout(
                *this,
                in_param,
                in_parent_window
                );
        }
    }
}

void UIHierarchyNodeChild::UpdateResources(
    const UIHierarchyNodeUpdateParam& in_param,
    const UIScreenSpace& in_parent_screen_space,
    const VectorInt2& in_parent_texture_size,
    const VectorInt2& in_parent_adjust_offset
    )
{
    if (nullptr != _component)
    {
        _component->UpdateResources(
            *this,
            in_param,
            in_parent_screen_space,
            in_parent_texture_size,
            _cache_parent_window,
            _cache_parent_offset,// + in_parent_adjust_offset,
            _layout.GetTextureMarginRef(),
            in_parent_adjust_offset
            );
    }
    return;
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
        SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
    }
    return;
}

void UIHierarchyNodeChild::UpdateScroll(
    const UIHierarchyNodeUpdateParam& in_param
    )
{
    const bool uv_scroll_manual_x = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollX));
    const bool uv_scroll_manual_y = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollY));

    DSC_ASSERT(nullptr != _node, "node should be passed into ctor, what happened");
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
    const VectorInt2& in_parent_texture_size,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset
    )
{
    const VectorInt2 texture_size = _node->GetTextureSize(in_param._draw_system);

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    IUIComponent::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        in_parent_texture_size,
        in_parent_window,
        in_parent_offset,
        //_adjust_offset,
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
#if 0
        SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
#else
        // geometry is what is used to draw to parent, if it changes, ie, auto scroll, it is the parent that needs to render
        if (nullptr != _parent_or_null)
        {
            _parent_or_null->SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
        }
#endif
    }

    _screen_space->Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    return;
}

void UIHierarchyNodeChild::DealInput(
    UIRootInputState& in_input_state,
    const bool in_parent_inside
    )
{
    if ((true == GetStateFlagBit(UIStateFlag::TDisable)) ||
        (true == GetStateFlagBit(UIStateFlag::THidden)))
    {
        return;
    }

    bool input_inside = false;
    if (nullptr != _component_input)
    {
        for (auto& touch : in_input_state.GetTouchArray())
        {
            const bool local_inside = _screen_space->GetClipRef().Inside(touch._touch_pos_current);

            if ((true == in_parent_inside) && 
                (true == local_inside))
            {
                input_inside = true;
            }
        }
    }

    // if input_inside starts to be problamatic for being for ANY touch, then move it under the touch data? 
    _node->DealInput(
        in_input_state,
        input_inside
        );

    if (nullptr != _component_input)
    {
        bool hover_flag = false;
        bool touch_flag = false;

        for (auto& touch : in_input_state.GetTouchArray())
        {
            std::string tooltip;
            const bool local_inside = _screen_space->GetClipRef().Inside(touch._touch_pos_current);

            if ((true == in_parent_inside) && 
                (true == local_inside))
            {
                // consume the first "active" touch, ie, mark touch for this source token
                if ((nullptr == touch._active_source_token) &&
                    (true == touch._active))
                {
                    touch._active_source_token = _source_token;
                }

                if ((nullptr == touch._active_source_token) || 
                    (touch._active_source_token == _source_token))
                {
                    hover_flag = true;

                    _component_input->OnHover(_screen_space->GetPosRef(), touch._touch_pos_current, tooltip);
                }

                // OnClick, needs to be over the same element as touch started on
                if ((true == touch._end) &&
                    (touch._active_source_token == _source_token))
                {
                    _component_input->OnInputClick(
                        in_input_state,
                        _screen_space->GetPosRef(),
                        touch._touch_pos_current
                        );
                }
            }

            if ((true == touch._active) &&
                (touch._active_source_token == _source_token) &&
                (false == touch._end))
            {
                touch_flag = true;

                _component_input->OnInputTouch(_screen_space->GetPosRef(), touch._touch_pos_current, tooltip);
            }

            if (false == tooltip.empty())
            {
                in_input_state.RequestTooltip(touch._touch_pos_current, _screen_space->GetClipRef(), tooltip, _source_token);
            }
        }

        // set the state flag, when we are a child with a component implementing IUIInput
        UIStateFlag pass_down_input_state_flag = UIStateFlag::TNone;
        if (true == hover_flag)
        {
            pass_down_input_state_flag = static_cast<UIStateFlag>(
                (static_cast<int>(pass_down_input_state_flag) | static_cast<int>(UIStateFlag::THover))
                );
        }
        if (true == touch_flag)
        {
            pass_down_input_state_flag = static_cast<UIStateFlag>(
                (static_cast<int>(pass_down_input_state_flag) | static_cast<int>(UIStateFlag::TTouch))
                );
        }

        // update state flag, just not via DealInputSetStateFlag which filters out nodes that implement IUIInput
        DealInputSetStateFlagImplement(pass_down_input_state_flag);
    }

    return;
}

void UIHierarchyNodeChild::DealInputSetStateFlag(const UIStateFlag in_input_state_flag)
{
    if (nullptr != _component_input)
    {
        return;
    }
    DealInputSetStateFlagImplement(in_input_state_flag);
}

void UIHierarchyNodeChild::DealInputSetStateFlagImplement(const UIStateFlag in_input_state_flag)
{
    SetStateFlag(static_cast<UIStateFlag>(
        (static_cast<int>(_state_flag) & ~(static_cast<int>(UIStateFlag::TMaskInput))) |
        static_cast<int>(in_input_state_flag)
        ));

    auto& node = GetNode();
    node.DealInputSetStateFlag(in_input_state_flag);
}

void UIHierarchyNodeChild::PreDraw(
    const UIManagerDrawParam& in_draw_param
    )
{
    LOG_MESSAGE_UI_VERBOSE("  UIHierarchyNodeChild::PreDraw %p %s %d", _source_token, _debug_name.c_str(), _state_dirty);

    if ((nullptr == _component) ||
        (true == GetStateFlagBit(UIStateFlag::THidden)))
    {
        return;
    }

    //bool dirty = false;
    if (true == GetStateDirtyBit(UIStateDirty::TRenderDirty))
    {
        auto& node = GetNode();
        if (nullptr != _component)
        {
            _component->PreDraw(in_draw_param, node);
        }

        node.Draw(
            in_draw_param,
            _state_flag,
            true
            );

        SetStateDirtyBit(UIStateDirty::TRenderDirty, false);
    }
    return;
}

void UIHierarchyNodeChild::Draw(
    const UIManagerDrawParam& in_draw_param
    )
{
    LOG_MESSAGE_UI_VERBOSE("  UIHierarchyNodeChild::Draw %p %s %d", _source_token, _debug_name.c_str(), _state_dirty);

    if ((nullptr == _component) ||
        (true == GetStateFlagBit(UIStateFlag::THidden)))
    {
        return;
    }

    const auto& shader = in_draw_param._ui_manager->GetShaderRef(UIShaderEnum::TDefault);

    DSC_ASSERT(nullptr != shader, "why is shader null");
    DSC_ASSERT(nullptr != _node, "node should be passed into ctor, what happened");
    if (false == _node->GetUITexture().SetShaderResource(
        *shader,
        0,
        in_draw_param._frame,
        in_draw_param._draw_system
        ))
    {
        return;
    }

    if (nullptr == _shader_constant_buffer)
    {
        _shader_constant_buffer = shader->MakeShaderConstantBuffer(
            in_draw_param._draw_system
            );
    }

    if (nullptr != _shader_constant_buffer)
    {
        UIManager::TShaderConstantBuffer& buffer = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
        buffer._tint_colour = CalculateTintColour();
    }

    in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

    DSC_ASSERT(nullptr != _geometry, "geometry should be passed into ctor, what happened");
    _geometry->Draw(
        in_draw_param._draw_system,
        in_draw_param._frame
        );

    return;
}
