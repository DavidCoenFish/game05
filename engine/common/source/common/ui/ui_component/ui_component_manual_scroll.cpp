#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_manual_scroll.h"

#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/ui/ui_component/ui_component_scroll.h"
#include "common/ui/ui_data/ui_data_manual_scroll.h"
#include "common/ui/ui_data/ui_data_scroll.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_shader_enum.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_enum.h"

UIComponentManualScroll::UIComponentManualScroll(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
    : _component_default(
        in_base_colour,
        in_layout
        )
    , _horizontal_scroll_wrapper(nullptr)
    , _vertical_scroll_wrapper(nullptr)
    , _horizontal_scroll(nullptr)
    , _vertical_scroll(nullptr)
{
    // Nop
}

UIComponentManualScroll::~UIComponentManualScroll()
{
    // Nop
}

const bool UIComponentManualScroll::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
{
    bool dirty = false;
    if (true == _component_default.SetBase(
        in_base_colour,
        in_layout
        ))
    {
        dirty = true;
    }

    return dirty;
}

const bool UIComponentManualScroll::SetStateFlag(const UIStateFlag in_state_flag)
{
    bool dirty = false;
    if (true == _component_default.SetStateFlag(in_state_flag))
    {
        dirty = true;
    }

    return dirty;
}

const bool UIComponentManualScroll::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentManualScroll::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentManualScroll::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentManualScroll::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

void UIComponentManualScroll::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentManualScroll::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentManualScroll::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    UIDataManualScroll* const data = dynamic_cast<UIDataManualScroll*>(in_data);

    if (true == _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        ))
    {
        dirty = true;
    }

    auto& child_data_array = in_out_child_data._node->GetChildData();
    UIHierarchyNodeChildData* const horizontal_child_data = 2 <= child_data_array.size() ? child_data_array[1].get() : nullptr;
    _horizontal_scroll_wrapper = nullptr;
    if (nullptr != horizontal_child_data)
    {
        _horizontal_scroll_wrapper = horizontal_child_data->_component.get();
        _horizontal_scroll = nullptr;
        UIDataScroll* const scroll = data ? data->GetHorizontalScroll() : nullptr;
        horizontal_child_data->VisitComponents([this, scroll](IUIComponent* const in_component){
                if (scroll == in_component->GetSourceToken())
                {
                    _horizontal_scroll = dynamic_cast<UIComponentScroll*>(in_component);
                    return false;
                }
                return true;
            });
    }

    UIHierarchyNodeChildData* const vertical_child_data = 3 <= child_data_array.size() ? child_data_array[2].get() : nullptr;
    _vertical_scroll_wrapper = nullptr;
    if (nullptr != vertical_child_data)
    {
        _vertical_scroll_wrapper = vertical_child_data->_component.get();
        _vertical_scroll = nullptr;
        UIDataScroll* const scroll = data ? data->GetVerticalScroll() : nullptr;

        vertical_child_data->VisitComponents([this, scroll](IUIComponent* const in_component){
                if (scroll == in_component->GetSourceToken())
                {
                    _vertical_scroll = dynamic_cast<UIComponentScroll*>(in_component);
                    return false;
                }
                return true;
            });
    }

    return dirty;
}

const bool UIComponentManualScroll::UpdateSize(
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
    // what size this layout wants in the parent layout
    VectorInt2 layout_size;
    VectorInt2 desired_size;
    GetDesiredSize(
        layout_size,
        desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );

    // instead of shrinking our desired size, use it as the initial value of the max_child_size
    VectorInt2 max_child_size = desired_size;

    // first child of the manual scroll is the content we care about the size of
    auto& child_array = in_out_node.GetChildData();
    if ((0 < child_array.size()) && (nullptr != child_array[0]))
    {
        UIHierarchyNodeChildData& child_data = *child_array[0];
        if (nullptr != child_data._component)
        {
            VectorInt2 child_layout_size;
            VectorInt2 child_desired_size;
            child_data._component->GetDesiredSize(
                child_layout_size,
                child_desired_size,
                layout_size, 
                in_ui_scale, 
                *child_data._node
                );
            max_child_size = VectorInt2::Max(max_child_size, child_desired_size);
        }
    }

    VectorFloat2 uv_scroll;

    // adjust our desired size for children, if allowed to scroll
    if (nullptr != _horizontal_scroll_wrapper)
    {
        if (layout_size[0] < max_child_size[0])
        {
            desired_size[0] = max_child_size[0];
            if (nullptr != _horizontal_scroll)
            {
                float ratio = _horizontal_scroll->GetValueRatio();
                uv_scroll[0] = ratio;
                const VectorFloat2 range(0.0f, static_cast<float>(max_child_size[0]));
                const float inner_length = static_cast<float>(layout_size[0]);
                const float start = ratio * (range[1] - inner_length);
                const VectorFloat2 value(start, start + inner_length);
                auto on_value_change = _horizontal_scroll->GetOnValueChange();
                if (nullptr != on_value_change)
                {
                    on_value_change(value);
                }
                auto on_range_change = _horizontal_scroll->GetOnRangeChange();
                if (nullptr != on_range_change)
                {
                    on_range_change(range);
                }
                _horizontal_scroll->Set(
                    on_value_change,
                    on_range_change,
                    value,
                    range
                    );
            }
            _horizontal_scroll_wrapper->SetStateFlagBit(UIStateFlag::THidden, false);
        }
        else
        {
            _horizontal_scroll_wrapper->SetStateFlagBit(UIStateFlag::THidden, true);
        }
    }

    if (nullptr != _vertical_scroll_wrapper)
    {
        if (layout_size[1] < max_child_size[1])
        {
            desired_size[1] = max_child_size[1];
            if (nullptr != _vertical_scroll)
            {
                float ratio = _vertical_scroll->GetValueRatio();
                uv_scroll[1] = ratio;
                const VectorFloat2 range(0.0f, static_cast<float>(max_child_size[1]));
                const float inner_length = static_cast<float>(layout_size[1]);
                const float start = ratio * (range[1] - inner_length);
                const VectorFloat2 value(start, start + inner_length);

                auto on_value_change = _vertical_scroll->GetOnValueChange();
                if (nullptr != on_value_change)
                {
                    on_value_change(value);
                }
                auto on_range_change = _vertical_scroll->GetOnRangeChange();
                if (nullptr != on_range_change)
                {
                    on_range_change(range);
                }

                _vertical_scroll->Set(
                    // The advantage of obliging _vertical_scroll to implement the on change, is that it can have persistant scope, unlike UIComponentManualScroll, ie, it is in the model
                    on_value_change,
                    on_range_change,
                    value,
                    range
                    );
            }
            _vertical_scroll_wrapper->SetStateFlagBit(UIStateFlag::THidden, false);
        }
        else
        {
            _vertical_scroll_wrapper->SetStateFlagBit(UIStateFlag::THidden, true);
        }
    }

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    UIComponentDefault::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        uv_scroll,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        0.0f, //in_time_delta, is there a better way to disable auto scroll?
        layout_size,
        desired_size,
        _component_default.GetLayout()
        );

    // Update geometry
    bool dirty = false;
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
        texture_size, 
        VectorInt2(),
        texture_size,
        in_ui_scale,
        in_time_delta,
        dirty,
        out_screen_space
        );

    return dirty;
}

void UIComponentManualScroll::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );
    return;
}

const bool UIComponentManualScroll::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _component_default.PreDraw(
        in_draw_param,
        in_node
        );
}

