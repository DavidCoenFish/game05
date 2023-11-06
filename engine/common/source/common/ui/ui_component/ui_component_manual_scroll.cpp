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
    const UILayout& in_layout,
    const bool in_allow_horizontal_scroll,
    const bool in_allow_vertical_scroll
    )
    : _component_default(
        in_base_colour,
        in_layout
        )
    , _allow_horizontal_scroll(in_allow_horizontal_scroll)
    , _allow_vertical_scroll(in_allow_vertical_scroll)
    , _do_horizontal_scroll(false)
    , _do_vertical_scroll(false)
{
    _child_data_vertical_scroll = UIHierarchyNodeChildData::Factory();
    _child_data_horizontal_scroll = UIHierarchyNodeChildData::Factory();
}

UIComponentManualScroll::~UIComponentManualScroll()
{
    // Nop
}

const bool UIComponentManualScroll::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const bool in_allow_horizontal_scroll,
    const bool in_allow_vertical_scroll    
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

    if (_allow_horizontal_scroll != in_allow_horizontal_scroll)
    {
        _allow_horizontal_scroll = in_allow_horizontal_scroll;
        dirty = true;
    }

    if (_allow_vertical_scroll != in_allow_vertical_scroll)
    {
        _allow_vertical_scroll = in_allow_vertical_scroll;
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

    if (true == UIHierarchyNodeChildData::RecurseSetStateFlagInput(_child_data_vertical_scroll.get(), in_state_flag))
    {
        dirty = true;
    }

    if (true == UIHierarchyNodeChildData::RecurseSetStateFlagInput(_child_data_horizontal_scroll.get(), in_state_flag))
    {
        dirty = true;
    }

    return dirty;
}

const UIStateFlag UIComponentManualScroll::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentManualScroll::GetLayout() const
{
    return _component_default.GetLayout();
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

    {
        UIHierarchyNodeChildData* child_data = _child_data_horizontal_scroll.get();
        UIData* const inner_data = data ? data->GetHorizontalScrollWrapper() : nullptr;
        if (nullptr != child_data)
        {
            if (true == child_data->ApplyFactory(inner_data, in_param, 0))
            {
                dirty = true;
            }
        }

        _horizontal_scroll = nullptr;
        UIDataScroll* const scroll = data ? data->GetHorizontalScroll() : nullptr;
        child_data->VisitComponents([this, scroll](IUIComponent* const in_component){
                if (scroll == in_component->GetSourceToken())
                {
                    _horizontal_scroll = dynamic_cast<UIComponentScroll*>(in_component);
                    return false;
                }
                return true;
            });
    }

    {
        UIHierarchyNodeChildData* child_data = _child_data_vertical_scroll.get();
        UIData* const inner_data = data ? data->GetVerticalScrollWrapper() : nullptr;
        if (nullptr != child_data)
        {
            if (true == child_data->ApplyFactory(inner_data, in_param, 0))
            {
                dirty = true;
            }
        }

        _vertical_scroll = nullptr;
        UIDataScroll* const scroll = data ? data->GetVerticalScroll() : nullptr;
        child_data->VisitComponents([this, scroll](IUIComponent* const in_component){
                if (scroll == in_component->GetSourceToken())
                {
                    _vertical_scroll = dynamic_cast<UIComponentScroll*>(in_component);
                    return false;
                }
                return true;
            });
    }

    if (true == _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        ))
    {
        dirty = true;
    }

    return dirty;
}

void UIComponentManualScroll::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space,
    UILayout* const in_layout_override
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
        in_out_node,
        in_layout_override
        );

    // instead of shrinking our desired size, use it as the initial value of the max_child_size
    //desired_size = layout_size;

    VectorInt2 max_child_size = desired_size;
    // now, of "our" children, what is the biggest needed size
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
            layout_size, 
            in_ui_scale, 
            *child_data._node
            );
        max_child_size = VectorInt2::Max(max_child_size, child_desired_size);
    }

    VectorFloat2 uv_scroll;

    // adjust our desired size for children, if allowed to scroll
    _do_horizontal_scroll = false;
    if ((true == _allow_horizontal_scroll) && (layout_size[0] < max_child_size[0]))
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
            _horizontal_scroll->Set(
                _horizontal_scroll->GetOnValueChange(),
                value,
                range
                );
            _do_horizontal_scroll = true;
        }
    }
    _do_vertical_scroll = false;
    if ((true == _allow_vertical_scroll) && (layout_size[1] < max_child_size[1]))
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
            _vertical_scroll->Set(
                // The advantage of obliging _vertical_scroll to implement the on change, is that it can have persistant scope, unlike UIComponentManualScroll, ie, it is in the model
                _vertical_scroll->GetOnValueChange(),
                value,
                range
                );
            _do_vertical_scroll = true;
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
        in_time_delta,
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

    if ((nullptr != _child_data_horizontal_scroll) && (nullptr != _child_data_horizontal_scroll->_component))
    {
        _child_data_horizontal_scroll->_component->UpdateSize(
            in_draw_system,
            texture_size, //in_parent_size,
            VectorInt2(), //in_parent_offset,
            texture_size, //in_parent_window,
            in_ui_scale,
            in_time_delta, 
            *_child_data_horizontal_scroll->_geometry, 
            *_child_data_horizontal_scroll->_node,
            in_parent_screen_space,
            *_child_data_horizontal_scroll->_screen_space
            );
    }

    if ((nullptr != _child_data_vertical_scroll) && (nullptr != _child_data_vertical_scroll->_component))
    {
        _child_data_vertical_scroll->_component->UpdateSize(
            in_draw_system,
            texture_size, //in_parent_size,
            VectorInt2(), //in_parent_offset,
            texture_size, //in_parent_window,
            in_ui_scale,
            in_time_delta, 
            *_child_data_vertical_scroll->_geometry, 
            *_child_data_vertical_scroll->_node,
            in_parent_screen_space,
            *_child_data_vertical_scroll->_screen_space
            );
    }

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

    return;
}

void UIComponentManualScroll::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    UILayout* const in_layout_override
    )
{
    _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        in_layout_override
        );
    return;
}

const bool UIComponentManualScroll::Draw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    bool dirty = false;

    if ((nullptr != _child_data_horizontal_scroll) && (nullptr != _child_data_horizontal_scroll->_component) && (true == _do_horizontal_scroll))
    {
        if (true == _child_data_horizontal_scroll->_component->Draw(
            in_draw_param,
            *(_child_data_horizontal_scroll->_node.get())
            ))
        {
            dirty = true;
        }
    }

    if ((nullptr != _child_data_vertical_scroll) && (nullptr != _child_data_vertical_scroll->_component) && (true == _do_vertical_scroll))
    {
        if (true == _child_data_vertical_scroll->_component->Draw(
            in_draw_param,
            *(_child_data_vertical_scroll->_node.get())
            ))
        {
            dirty = true;
        }
    }

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
        if ((nullptr != _child_data_horizontal_scroll) && (true == _do_horizontal_scroll))
        {
            _child_data_horizontal_scroll->Draw(in_draw_param);
        }

        if ((nullptr != _child_data_vertical_scroll) && (true == _do_vertical_scroll))
        {
            _child_data_vertical_scroll->Draw(in_draw_param);
        }

        dirty = true;
    }

    return dirty;

}
