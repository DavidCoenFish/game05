#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_default.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/log/log.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_data/ui_data.h"

#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_stack.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/log/log.h"

//        LOG_CONSOLE("SetStateFlag %d => %d", _state_flag, in_state_flag);

namespace
{
    const float CalculatePos(const int in_pos, const int in_size)
    {
        return ((static_cast<float>(in_pos) / static_cast<float>(in_size)) * 2.0f) - 1.0f;
    }

    constexpr float s_scroll_speed = 5.0f;
}

UIComponentDefault::UIComponentDefault(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    void* in_source_token,
    const UIStateFlag in_state_flag
    )
    : _base_colour(in_base_colour)
    , _layout(in_layout)
    , _state_flag_tint_array(in_state_flag_tint_array)
    , _source_token(in_source_token)
    , _time_accumulate_seconds(0.0f)
    , _state_flag(in_state_flag)
    , _state_flag_dirty_mask(UIStateFlag::TNone)
    , _use_layout_override(false)
    , _layout_override()
    , _uv_scroll_manual_x(false)
    , _uv_scroll_manual_y(false)
{
    if (nullptr != _state_flag_tint_array)
    {
        SetStateFlagDirty(UIStateFlag::TTintMask);
    }
    else
    {
        SetStateFlagDirty(UIStateFlag::TNone);
    }
}

UIComponentDefault::~UIComponentDefault()
{
    // Nop
}

void UIComponentDefault::CalculateGeometry(
    VectorFloat4& out_geometry_pos,
    VectorFloat4& out_geometry_uv,
    VectorInt2& out_texture_size,
    VectorFloat2& in_out_scroll,
    const bool in_uv_scroll_manual_x,
    const bool in_uv_scroll_manual_y,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    const VectorInt2& in_layout_size, // layout size != in_parent_window, unless layout size is 100% of the parent window...
    const VectorInt2& in_desired_size,
    const UILayout& in_layout 
    )
{
    const VectorInt2 pivot = in_layout.GetPivot(in_parent_window, in_ui_scale) + in_parent_offset; 
    const VectorInt2 attach = in_layout.GetAttach(in_layout_size, in_ui_scale);

    // Deal pos
    //VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f)
    out_geometry_pos = VectorFloat4(
        CalculatePos(pivot[0] - attach[0], in_parent_size[0]),
        CalculatePos(pivot[1] - attach[1], in_parent_size[1]),
        CalculatePos(pivot[0] + in_layout_size[0] - attach[0], in_parent_size[0]),
        CalculatePos(pivot[1] + in_layout_size[1] - attach[1], in_parent_size[1])
        );

    // Deal uv
    out_geometry_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f);
    const VectorInt2 delta(
        in_desired_size[0] - in_layout_size[0],
        in_desired_size[1] - in_layout_size[1]
        );
    if (0 < delta[0])
    {
        if (false == in_uv_scroll_manual_x)
        {
            const float delta_f = static_cast<float>(delta[0]);
            float new_value = in_out_scroll[0] + (s_scroll_speed * in_time_delta / delta_f);
            new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
            in_out_scroll[0] = new_value;
        }

        const float length = (static_cast<float>(in_layout_size[0]) / static_cast<float>(in_desired_size[0]));//
        const float offset = (1.0f - length) * abs(in_out_scroll[0]);
        out_geometry_uv[0] = offset;
        out_geometry_uv[2] = offset + length;
    }

    if (0 < delta[1])
    {
        if (false == in_uv_scroll_manual_y)
        {
            const float delta_f = static_cast<float>(delta[1]);
            float new_value = in_out_scroll[1] + (s_scroll_speed * in_time_delta / delta_f);
            new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
            in_out_scroll[1] = new_value;
        }

        const float length = (static_cast<float>(in_layout_size[1]) / static_cast<float>(in_desired_size[1]));//
        const float offset = (1.0f - length) * abs(in_out_scroll[1]);
        out_geometry_uv[1] = offset + length;
        out_geometry_uv[3] = offset;
    }

    // Deal texture
    out_texture_size = VectorInt2::Max(in_layout_size, in_desired_size);

    return;
}

const bool UIComponentDefault::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
{
    bool dirty = false;

    if (_base_colour != in_base_colour)
    {
        dirty = true;
        _base_colour = in_base_colour;
    }

    if (_layout != in_layout)
    {
        dirty = true;
        _layout == in_layout;
        _uv_scroll = VectorFloat2();
    }

    if (_state_flag_tint_array != in_state_flag_tint_array)
    {
        dirty = true;
        _state_flag_tint_array = in_state_flag_tint_array;

        if (nullptr != _state_flag_tint_array)
        {
            SetStateFlagDirty(UIStateFlag::TTintMask);
        }
        else
        {
            SetStateFlagDirty(UIStateFlag::TNone);
        }
    }

    return dirty;
}

// Make sorting children easier
void UIComponentDefault::SetSourceToken(void* in_source_ui_data_token)
{
    _source_token = in_source_ui_data_token;
}

void* UIComponentDefault::GetSourceToken() const
{
    return _source_token;
}

void UIComponentDefault::SetLayoutOverride(const UILayout& in_override)
{
    _use_layout_override = true;
    #if 0
    if (_layout_override != in_override)
    {
        LOG_CONSOLE("LayoutOverride Change");
    }
    #endif

    _layout_override = in_override;
    return;
}

const UILayout& UIComponentDefault::GetInUseLayout() const
{
    if (true == _use_layout_override)
    {
        return _layout_override;
    }
    return _layout;
}

void UIComponentDefault::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
{
    _uv_scroll_manual_x = in_manual_horizontal;
    _uv_scroll_manual_y = in_manual_vertical;
    if (true == _uv_scroll_manual_x)
    {
        _uv_scroll[0] = in_uv_scroll[0];
    }
    if (true == _uv_scroll_manual_y)
    {
        _uv_scroll[1] = in_uv_scroll[1];
    }
    return;
}

/// return true if any bits under the dirty flag are modified
const bool UIComponentDefault::SetStateFlag(const UIStateFlag in_state_flag)
{
    const bool dirty = (static_cast<int>(_state_flag) & static_cast<int>(_state_flag_dirty_mask)) 
        != (static_cast<int>(in_state_flag) & static_cast<int>(_state_flag_dirty_mask));
    #if 0
    if (true == dirty)
    {
        LOG_CONSOLE("SetStateFlag %d => %d", _state_flag, in_state_flag);
    }
    #endif
    _state_flag = in_state_flag;
    return dirty;
}

const bool UIComponentDefault::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
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
    return SetStateFlag(static_cast<UIStateFlag>(state_flag));
}

const bool UIComponentDefault::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    if (nullptr != in_data)
    {
        if (true == in_data->VisitDataArray([
            &in_out_child_data,
            &in_param,
            this
            ](const std::vector<std::shared_ptr<UIData>>& in_array_data)->bool{
            return in_out_child_data._node->UpdateHierarchy(
                in_param,
                &in_array_data,
                true,
                false,
                _base_colour.GetClearBackground(),
                _base_colour.GetClearColourRef()
                );
        }))
        {
            dirty =true;
        }
    }
    else
    {
        if (true == in_out_child_data._node->UpdateHierarchy(
            in_param,
            nullptr,
            true,
            false,
            _base_colour.GetClearBackground(),
            _base_colour.GetClearColourRef()
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

const bool UIComponentDefault::Update(
    const float in_time_delta
    )
{
    const float prev_time = _time_accumulate_seconds;
    _time_accumulate_seconds += in_time_delta;
    const bool dirty = _base_colour.GetTimeChangeDirty(prev_time, _time_accumulate_seconds);
    return dirty;
}

const bool UIComponentDefault::UpdateSize(
    DrawSystem* const in_draw_system,
    IUIComponent& in_out_ui_component,
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
    bool dirty = false;

    if (true == Update(in_time_delta))
    {
        dirty = true;
    }

    VectorInt2 layout_size;
    VectorInt2 desired_size;
    in_out_ui_component.GetDesiredSize(
        layout_size,
        desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        _uv_scroll,
        _uv_scroll_manual_x,
        _uv_scroll_manual_y,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta,
        layout_size,
        desired_size,
        GetInUseLayout()
        );

    // Update geometry
    if (true == in_out_geometry.Set(
        geometry_pos,
        geometry_uv
        ))
    {
        dirty = true;
    }

    out_screen_space.Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    // Recurse
    in_out_node.UpdateSize(
        in_draw_system,
        texture_size, // by default, we use the generated size and scroll if required. a stack control may modify this
        VectorInt2(),
        texture_size,
        in_ui_scale,
        in_time_delta,
        dirty,
        out_screen_space
        );

    return dirty;
}

void UIComponentDefault::GetDesiredSize(
    VectorInt2& out_layout_size,
    VectorInt2& out_desired_size,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& //in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    out_layout_size = _use_layout_override ? _layout_override.GetSize(in_parent_window, in_ui_scale) 
        : _layout.GetSize(in_parent_window, in_ui_scale);
    out_desired_size = out_layout_size;
    return;
}

const bool UIComponentDefault::PreDraw(
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
    if (true == in_node.Draw(
        in_draw_param,
        dirty
        ))
    {
        dirty = true;
    }

    return dirty;
}

const VectorFloat4 UIComponentDefault::GetTintColour() const
{
    const VectorFloat4* override_tint = nullptr;
    const TStateFlagTintArray* const state_flag_tint_array = _state_flag_tint_array.get();

    if (nullptr != state_flag_tint_array)
    {
        const int tint_index = static_cast<int>(GetStateFlag()) & static_cast<int>(UIStateFlag::TTintMask);
        override_tint = &(*state_flag_tint_array)[tint_index];
    }

    return _base_colour.GetTintColour(_time_accumulate_seconds, override_tint);
}

