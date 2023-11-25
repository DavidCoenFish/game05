#include "common/common_pch.h"
#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_data/ui_data.h"
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
    void* in_source_token,
    const UIStateFlag in_state_flag
    )
    : _layout(in_layout)
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

void IUIComponent::CalculateGeometry(
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
//
//const bool IUIComponent::SetModel(
//    const UIBaseColour& in_base_colour,
//    const UILayout& in_layout,
//    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
//    )
//{
//    bool dirty = false;
//
//    if (_base_colour != in_base_colour)
//    {
//        dirty = true;
//        _base_colour = in_base_colour;
//    }
//
//    // base (model) layout changes only count when we are not in layout override mode
//    if (0 == (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TLayoutOverride)))
//    {
//        if (_layout != in_layout)
//        {
//            dirty = true;
//            _layout == in_layout;
//            _uv_scroll = VectorFloat2();
//        }
//    }
//
//    if (_state_flag_tint_array != in_state_flag_tint_array)
//    {
//        dirty = true;
//        _state_flag_tint_array = in_state_flag_tint_array;
//
//        if (nullptr != _state_flag_tint_array)
//        {
//            SetStateFlagDirty(UIStateFlag::TTintMask);
//        }
//        else
//        {
//            SetStateFlagDirty(UIStateFlag::TNone);
//        }
//    }
//
//    return dirty;
//}

// Make sorting children easier
void IUIComponent::SetSourceToken(void* in_source_ui_data_token)
{
    _source_token = in_source_ui_data_token;
}

void* IUIComponent::GetSourceToken() const
{
    return _source_token;
}

const bool IUIComponent::CheckLayoutCache(
    VectorInt2& out_layout_size, 
    VectorInt2& out_desired_size, 
    const VectorInt2& in_parent_window
    )
{
    if (_cache_parent_window == in_parent_window)
    {
        out_layout_size = _cache_layout_size;
        out_desired_size = _cache_desired_size;
        return true;
    }
    return false;
}

void IUIComponent::SetLayoutCache(
    const VectorInt2& in_layout_size, 
    const VectorInt2& in_desired_size, 
    const VectorInt2& in_parent_window
    )
{
    _cache_layout_size = in_layout_size;
    _cache_desired_size = in_desired_size;
    _cache_parent_window = in_parent_window;
    return;
}

void IUIComponent::UpdateLayout(
    UIHierarchyNodeChildData& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window
    )
{
}

const VectorInt2 IUIComponent::GetDesiredSize(
    const UIHierarchyNodeUpdateParam& in_layout_param,
    const VectorInt2& in_pre_shrink_layout_size //in_parent_window
    )
{
    return VectorInt2::s_zero;
}

void IUIComponent::UpdateResources(
    UIHierarchyNodeChildData& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const UIScreenSpace& in_parent_screen_space
    )
{
}

const VectorInt2 IUIComponent::GetTextureSize()
{
    return _cache_desired_size;
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

/// Set layout dirty flag on change
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

//const bool IUIComponent::UpdateHierarchy(
//    UIData* const in_data,
//    UIHierarchyNodeChildData& in_out_child_data,
//    const UIHierarchyNodeUpdateHierarchyParam& in_param
//    )
//{
//    bool dirty = false;
//    if (nullptr != in_data)
//    {
//        if (true == in_data->VisitDataArray([
//            &in_out_child_data,
//            &in_param,
//            this
//            ](const std::vector<std::shared_ptr<UIData>>& in_array_data)->bool{
//            return in_out_child_data._node->UpdateHierarchy(
//                in_param,
//                &in_array_data,
//                true,
//                false,
//                _base_colour.GetClearBackground(),
//                _base_colour.GetClearColourRef()
//                );
//        }))
//        {
//            dirty =true;
//        }
//    }
//    else
//    {
//        if (true == in_out_child_data._node->UpdateHierarchy(
//            in_param,
//            nullptr,
//            true,
//            false,
//            _base_colour.GetClearBackground(),
//            _base_colour.GetClearColourRef()
//            ))
//        {
//            dirty = true;
//        }
//    }
//
//    if (0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TDirty)))
//    {
//        dirty = true;
//        SetStateFlagBit(UIStateFlag::TDirty, false);
//    }
//
//    return dirty;
//}
//
//const bool IUIComponent::UpdateSize(
//    DrawSystem* const in_draw_system,
//    const VectorInt2& in_parent_size,
//    const VectorInt2& in_parent_offset,
//    const VectorInt2& in_parent_window,
//    const float in_ui_scale,
//    const float in_time_delta, 
//    UIGeometry& in_out_geometry, 
//    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
//    const UIScreenSpace& in_parent_screen_space,
//    UIScreenSpace& out_screen_space
//    )
//{
//    bool dirty = false;
//
//    if (true == Update(in_time_delta))
//    {
//        dirty = true;
//    }
//
//    VectorInt2 layout_size;
//    VectorInt2 desired_size;
//    GetDesiredSize(
//        layout_size,
//        desired_size,
//        in_parent_window,
//        in_ui_scale,
//        in_out_node
//        );
//
//    VectorFloat4 geometry_pos;
//    VectorFloat4 geometry_uv;
//    VectorInt2 texture_size;
//
//    const bool uv_scroll_manual_x = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollX));
//    const bool uv_scroll_manual_y = 0 != (static_cast<int>(_state_flag) & static_cast<int>(UIStateFlag::TManualScrollY));
//
//    CalculateGeometry(
//        geometry_pos,
//        geometry_uv,
//        texture_size,
//        _uv_scroll,
//        uv_scroll_manual_x,
//        uv_scroll_manual_y,
//        in_parent_size,
//        in_parent_offset,
//        in_parent_window,
//        in_ui_scale,
//        in_time_delta,
//        layout_size,
//        desired_size,
//        _layout
//        );
//
//    // Update geometry
//    if (true == in_out_geometry.Set(
//        geometry_pos,
//        geometry_uv
//        ))
//    {
//        dirty = true;
//    }
//
//    out_screen_space.Update(
//        in_parent_screen_space,
//        geometry_pos,
//        geometry_uv
//        );
//
//    // Recurse
//    in_out_node.UpdateSize(
//        in_draw_system,
//        texture_size, // by default, we use the generated size and scroll if required. a stack control may modify this
//        VectorInt2(),
//        texture_size,
//        in_ui_scale,
//        in_time_delta,
//        dirty,
//        out_screen_space
//        );
//
//    return dirty;
//}
//
//void IUIComponent::GetDesiredSize(
//    VectorInt2& out_layout_size,
//    VectorInt2& out_desired_size,
//    const VectorInt2& in_parent_window,
//    const float in_ui_scale,
//    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
//    )
//{
//    if (((true == _layout.GetShrinkWidth()) || (true == _layout.GetShrinkHeight()))
//        && (0 < in_out_node.GetChildData().size()))
//    {
//        VectorInt2 max_child_desired;
//        VectorInt2 max_child_layout;
//        for (auto& child : in_out_node.GetChildData())
//        {
//            IUIComponent* const child_component = child ? child->_component.get() : nullptr;
//            if ((nullptr == child_component) || (nullptr == child->_node))
//            {
//                continue;
//            }
//            VectorInt2 child_layout_size;
//            VectorInt2 child_desired_size;
//
//            child_component->GetDesiredSize(
//                child_layout_size,
//                child_desired_size,
//                in_parent_window,
//                in_ui_scale,
//                *child->_node
//                );
//
//            max_child_layout = VectorInt2::Max(max_child_layout, child_layout_size);
//            max_child_desired = VectorInt2::Max(max_child_desired, child_desired_size);
//        }
//
//        if ((0 < max_child_desired.GetX()) && (0 < max_child_desired.GetY()))
//        {
//            //desired or layout?
//            // desired has assert on UIScale != 1.0f for effect size of tooltip dropshadow geometry not being full size
//            //out_layout_size = _layout.GetSize(max_child_desired, in_ui_scale); 
//            out_layout_size = _layout.GetSize(max_child_layout, in_ui_scale);
//            out_desired_size = out_layout_size;
//            return;
//        }
//    }
//    out_layout_size = _layout.GetSize(in_parent_window, in_ui_scale);
//    out_desired_size = out_layout_size;
//    return;
//}

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
        dirty
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
    const bool dirty = false; //_base_colour.GetTimeChangeDirty(prev_time, _time_accumulate_seconds);
    return dirty;
}
