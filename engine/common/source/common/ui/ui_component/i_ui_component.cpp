#include "common/common_pch.h"
#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_effect/ui_effect_component.h"
#include "common/ui/ui_effect/ui_effect_data.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_screen_space.h"


namespace
{
    const float CalculatePos(const int in_pos, const int in_size)
    {
        return ((static_cast<float>(in_pos) / static_cast<float>(in_size)) * 2.0f) - 1.0f;
    }

    constexpr float s_scroll_speed = 5.0f;
}

IUIComponent::IUIComponent(
    const UILayout& in_layout,
    const UITintColour& in_tint_colour,
    void* in_source_token,
    const UIStateFlag in_state_flag
    )
    : _layout(in_layout)
    , _tint_colour(in_tint_colour)
    , _source_token(in_source_token)
    , _time_accumulate_seconds(0.0f)
    , _state_flag(in_state_flag)
{
    // Nop
}

IUIComponent::~IUIComponent()
{
    // Nop
}

void IUIComponent::UpdateScroll(
    VectorFloat2& in_out_scroll,
    const bool in_scroll_manual_x,
    const bool in_scroll_manual_y,
    const float in_time_delta,
    const VectorInt2& in_layout_size,
    const VectorInt2& in_texture_size
    )
{
    const VectorInt2 delta(
        in_texture_size[0] - in_layout_size[0],
        in_texture_size[1] - in_layout_size[1]
        );
    if (0 < delta[0])
    {
        if (false == in_scroll_manual_x)
        {
            const float delta_f = static_cast<float>(delta[0]);
            float new_value = in_out_scroll[0] + (s_scroll_speed * in_time_delta / delta_f);
            new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
            in_out_scroll[0] = new_value;
        }
    }

    if (0 < delta[1])
    {
        if (false == in_scroll_manual_y)
        {
            const float delta_f = static_cast<float>(delta[1]);
            float new_value = in_out_scroll[1] + (s_scroll_speed * in_time_delta / delta_f);
            new_value = fmodf(new_value + 1.0f, 2.0f) - 1.0f; // want range [-1.0 ... 1.0]
            in_out_scroll[1] = new_value;
        }
    }

    return;
}

void IUIComponent::CalculateGeometry(
    VectorFloat4& out_geometry_pos,
    VectorFloat4& out_geometry_uv,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_layout_offset,
    const VectorInt2& in_layout_size,
    const VectorInt2& in_texture_size,
    const VectorFloat2& in_scroll,
    const UILayout& in_layout 
    )
{
    const VectorInt2 pivot = in_layout.GetParentWindowPivot(in_parent_size, in_layout_offset); 
    const VectorInt2 attach = in_layout.GetLayoutAttach(in_layout_size);

    // Deal pos
    //VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f)
    out_geometry_pos = VectorFloat4(
        CalculatePos(pivot[0] - attach[0], in_parent_size[0]),
        CalculatePos(pivot[1] - attach[1], in_parent_size[1]),
        CalculatePos(pivot[0] + in_layout_size[0] - attach[0], in_parent_size[0]),
        CalculatePos(pivot[1] + in_layout_size[1] - attach[1], in_parent_size[1])
        );

    out_geometry_uv = VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f);
    const VectorInt2 delta(
        in_texture_size[0] - in_layout_size[0],
        in_texture_size[1] - in_layout_size[1]
        );
    if (0 < delta[0])
    {
        const float length = (static_cast<float>(in_layout_size[0]) / static_cast<float>(in_texture_size[0]));
        const float offset = (1.0f - length) * abs(in_scroll[0]);
        out_geometry_uv[0] = offset;
        out_geometry_uv[2] = offset + length;
    }

    if (0 < delta[1])
    {
        const float length = (static_cast<float>(in_layout_size[1]) / static_cast<float>(in_texture_size[1]));
        const float offset = (1.0f - length) * abs(in_scroll[1]);
        out_geometry_uv[1] = offset + length;
        out_geometry_uv[3] = offset;
    }

    return;
}

void IUIComponent::SetTintColour(const UITintColour& in_tint_colour)
{
    if (_tint_colour != in_tint_colour)
    {
        _tint_colour = in_tint_colour;
        SetStateFlagBit(UIStateFlag::TDirty, true);
    }

    return;
}

const VectorFloat4 IUIComponent::CalculateTintColour() const
{
    return _tint_colour.GetTintColour(_time_accumulate_seconds);
}

// Make sorting children easier
void IUIComponent::SetSourceToken(void* in_source_ui_data_token)
{
    _source_token = in_source_ui_data_token;
}

void* IUIComponent::GetSourceToken() const
{
    return _source_token;
}

//const bool IUIComponent::CheckLayoutCache(
//    VectorInt2& out_layout_size, 
//    VectorInt2& out_desired_size, 
//    const VectorInt2& in_parent_window
//    )
//{
//    if (_cache_parent_window == in_parent_window)
//    {
//        out_layout_size = _cache_layout_size;
//        out_desired_size = _cache_desired_size;
//        return true;
//    }
//    return false;
//}
//
//void IUIComponent::SetLayoutCache(
//    const VectorInt2& in_layout_size, 
//    const VectorInt2& in_desired_size, 
//    const VectorInt2& in_parent_window
//    )
//{
//    _cache_layout_size = in_layout_size;
//    _cache_desired_size = in_desired_size;
//    _cache_parent_window = in_parent_window;
//    return;
//}

//void IUIComponent::ApplyEffect(
//    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
//    const UIHierarchyNodeUpdateParam& in_param,
//    const int in_index
//    )
//{
//    bool change = false;
//    const int size = in_array_effect_data.size();
//    if (size != _effect_component_array.size())
//    {
//        change = true;
//        _effect_component_array.resize(size);
//    }
//
//    for (int index = 0; index < size; ++index)
//    {
//        if (true == in_array_effect_data[index]->ApplyComponent(
//            _effect_component_array[index],
//            in_param,
//            in_index
//            ))
//        {
//            change = true;
//        }
//    }
//
//    if (true == change)
//    {
//        SetStateFlagBit(UIStateFlag::TDirty, true);
//    }
//}

const VectorInt2 IUIComponent::GetDesiredSize(
    UIHierarchyNodeChildData&, //in_component_owner,
    const UIHierarchyNodeUpdateParam& in_layout_param,
    const VectorInt2& //in_parent_window
    )
{
    return _layout.ApplyMargin(
        VectorInt2::s_zero,
        in_layout_param._ui_scale
        );
}

/// default behavoiur, ie, canvas
/// node::update layout (in parent window)
///     for each child A0
///         component::update layout
///             calculate layout size given parent window
///             recurse node::update layout
///             component::desired(parent window or layout size? layout size may yet to be modified...)
///             finialise layout size (shrink/expand)
void IUIComponent::UpdateLayout(
    UIHierarchyNodeChildData& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset
    )
{
    // calculate layout size given parent window
    VectorInt2 base_layout_size = _layout.GetLayoutSize(in_parent_window, in_param._ui_scale);

    // recurse node::update layout
    in_component_owner._node->UpdateLayout(
        in_param,
        //_layout_offset,
        base_layout_size,
        VectorInt2::s_zero
        );

    // component::desired(parent window or layout size? layout size may yet to be modified...)
    const VectorInt2 base_desired_size = GetDesiredSize(in_component_owner, in_param, base_layout_size);

    VectorInt2 texture_size;

    // finialise layout size (shrink/expand) and work out the texture size (which may include the texture margin)
    _layout.Finalise(
        _layout_size,
        texture_size,
        _layout_offset,
        base_layout_size,
        base_desired_size,
        in_parent_offset
        );

    in_component_owner._node->SetTextureSize(texture_size);
}

const bool IUIComponent::UpdateResources(
    UIHierarchyNodeChildData& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const UIScreenSpace& in_parent_screen_space,
    const VectorInt2& in_parent_texture_size
    )
{
    bool dirty = false;

    if (true == Update(in_param._delta_time_seconds))
    {
        dirty = true;
    }

    const bool uv_scroll_manual_x = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollX));
    const bool uv_scroll_manual_y = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollY));

    const VectorInt2 texture_size = in_component_owner._node->GetTextureSize(in_param._draw_system);

    UpdateScroll(
        _uv_scroll,
        uv_scroll_manual_x,
        uv_scroll_manual_y,
        in_param._delta_time_seconds,
        _layout_size,
        texture_size
        );

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    CalculateGeometry(
        geometry_pos,
        geometry_uv,
        in_parent_texture_size,
        _layout_offset,
        _layout_size,
        texture_size,
        _uv_scroll,
        _layout
        );

    if (true == in_component_owner._geometry->Set(
        geometry_pos,
        geometry_uv
        ))
    {
        dirty = true;
    }

    in_component_owner._screen_space->Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    if (true == in_component_owner._node->UpdateResources(
        in_param,
        *in_component_owner._screen_space
        ))
    {
        dirty = true;
    }

    if (true == dirty)
    {
        SetStateFlagBit(UIStateFlag::TDirty, true);
    }

    return dirty;
}

void IUIComponent::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
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

/// return true if any bits under the dirty flag are modified
const bool IUIComponent::SetStateFlag(const UIStateFlag in_state_flag)
{
    const bool dirty = _state_flag != in_state_flag;
    #if 0
    if (true == dirty)
    {
        LOG_CONSOLE("SetStateFlag %d => %d", _state_flag, in_state_flag);
    }
    #endif
    _state_flag = in_state_flag;
    return dirty;
}

const bool IUIComponent::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
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

const bool IUIComponent::GetStateFlagBit(const UIStateFlag in_state_flag_bit) const
{
    return 0 != (static_cast<int>(_state_flag) & static_cast<int>(in_state_flag_bit));
}

/// Set layout dirty flag on change, or is that handled by UIData~ and at the conponent level, layout only set from UIData
void IUIComponent::SetLayout(
    const UILayout& in_layout
    )
{
    if (_layout != in_layout)
    {
        _layout == in_layout;
        // TODO: do we need to set up the parent change the layout dirty flag?
        SetStateFlagBit(UIStateFlag::TLayoutDirty, true);
    }

    return;
}

const bool IUIComponent::PreDraw(
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
        dirty,
        _state_flag
        ))
    {
        dirty = true;
    }

    return dirty;
}

const bool IUIComponent::Update(
    const float in_time_delta
    )
{
    const float prev_time = _time_accumulate_seconds;
    _time_accumulate_seconds += in_time_delta;
    const bool dirty = _tint_colour.GetTimeChangeDirty(prev_time, _time_accumulate_seconds);
    return dirty;
}
