#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_manual_scroll.h"

#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/log/log.h"
#include "common/ui/ui_component/ui_component_scroll.h"
#include "common/ui/ui_data/ui_data_manual_scroll.h"
#include "common/ui/ui_data/ui_data_scroll.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_enum.h"

UIComponentManualScroll::UIComponentManualScroll(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
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

const bool UIComponentManualScroll::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    UIDataManualScroll* const data = dynamic_cast<UIDataManualScroll*>(in_data);

    if (true == TSuper::UpdateHierarchy(
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
        horizontal_child_data->VisitComponents([this, scroll](IUIComponent* const in_component, UIHierarchyNode* const){
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

        vertical_child_data->VisitComponents([this, scroll](IUIComponent* const in_component, UIHierarchyNode* const){
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
    // the default _component_default.UpdateSize recurses, we do want to do that, but with custom uv scroll for the first child
    // else the default behaviour is canvas to draw all the children over the top of each other

    bool dirty = false; 
    if (true == Update(in_time_delta))
    {
        dirty = true;
    }

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
    UIHierarchyNodeChildData* const main_child_data = 0 < child_array.size() ? child_array[0].get() : nullptr;
    IUIComponent* const main_component =  main_child_data ? main_child_data->_component.get() : nullptr;
    if (nullptr != main_component)
    {
        VectorInt2 child_layout_size;
        VectorInt2 child_desired_size;
        main_component->GetDesiredSize(
            child_layout_size,
            child_desired_size,
            layout_size, 
            in_ui_scale, 
            *(main_child_data->_node)
            );
        max_child_size = VectorInt2::Max(max_child_size, child_desired_size);
    }

    VectorFloat2 uv_scroll;
    bool scroll_horizontal = false;
    bool scroll_vertical = false;

    // adjust our desired size for children, if allowed to scroll
    if (nullptr != _horizontal_scroll_wrapper)
    {
        if (layout_size[0] < max_child_size[0])
        {
            scroll_horizontal = true;

            // TODO: resolve duplication of calling both _horizontal_scroll->Set AND on_value_change
            if (nullptr != _horizontal_scroll)
            {
                float ratio = _horizontal_scroll->GetValueRatio();
                uv_scroll[0] = ratio;
                const VectorFloat2 range(0.0f, static_cast<float>(max_child_size[0]));
                const float inner_length = static_cast<float>(layout_size[0]);
                const float start = ratio * (range[1] - inner_length);
                const VectorFloat2 value(start, start + inner_length);
                auto on_value_change = _horizontal_scroll->GetOnValueChange();
                auto on_range_change = _horizontal_scroll->GetOnRangeChange();
                if (true == _horizontal_scroll->Set(
                    on_value_change,
                    on_range_change,
                    value,
                    range
                    ))
                {
                    dirty = true;
                    if (nullptr != on_value_change)
                    {
                        on_value_change(value);
                    }
                    if (nullptr != on_range_change)
                    {
                        on_range_change(range);
                    }
                }
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
            scroll_vertical = true;

            if (nullptr != _vertical_scroll)
            {
                float ratio = _vertical_scroll->GetValueRatio();
                uv_scroll[1] = ratio;
                const VectorFloat2 range(0.0f, static_cast<float>(max_child_size[1]));
                const float inner_length = static_cast<float>(layout_size[1]);
                const float start = ratio * (range[1] - inner_length);
                const VectorFloat2 value(start, start + inner_length);

                auto on_value_change = _vertical_scroll->GetOnValueChange();
                auto on_range_change = _vertical_scroll->GetOnRangeChange();

                if (true == _vertical_scroll->Set(
                    // The advantage of obliging _vertical_scroll to implement the on change, is that it can have persistant scope, unlike UIComponentManualScroll, ie, it is in the model
                    on_value_change,
                    on_range_change,
                    value,
                    range
                    ))
                {
                    //LOG_CONSOLE("vertical scroll value change [%f %f]", value[0], value[1]);
                    dirty = true;

                    if (nullptr != on_value_change)
                    {
                        on_value_change(value);
                    }
                    if (nullptr != on_range_change)
                    {
                        on_range_change(range);
                    }
                }
            }
            _vertical_scroll_wrapper->SetStateFlagBit(UIStateFlag::THidden, false);
        }
        else
        {
           _vertical_scroll_wrapper->SetStateFlagBit(UIStateFlag::THidden, true);
        }
    }

    if (nullptr != main_component)
    {
        main_component->SetUVScrollManual(uv_scroll, scroll_horizontal, scroll_vertical);
    }

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    const auto state_flag = GetStateFlag();
    const bool uv_scroll_manual_x = 0 != (static_cast<int>(state_flag) & static_cast<int>(UIStateFlag::TManualScrollX));
    const bool uv_scroll_manual_y = 0 != (static_cast<int>(state_flag) & static_cast<int>(UIStateFlag::TManualScrollY));

    CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        GetUVScroll(),
        uv_scroll_manual_x,
        uv_scroll_manual_y,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta,
        layout_size,
        desired_size,
        GetLayout()
        );

    // Update geometry
    if (true == in_out_geometry.Set(
        geometry_pos,
        geometry_uv
        ))
    {
        //LOG_CONSOLE("manual scroll geometry dirty pos:[%f %f %f %f] uv:[%f %f %f %f]", 
        //    geometry_pos[0], geometry_pos[1], geometry_pos[2], geometry_pos[3],
        //    geometry_uv[0], geometry_uv[1], geometry_uv[2], geometry_uv[3]);
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
