#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_stack.h"

#include "common/math/vector_int4.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_enum.h"

UIComponentStack::UIComponentStack(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const UIOrientation in_orientation,
    const UICoord& in_gap
    )
    : _component_default(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    , _orientation(in_orientation)
    , _gap(in_gap)
{
    // Nop
}

UIComponentStack::~UIComponentStack()
{
    // Nop
}

const bool UIComponentStack::Set(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array,
    const UIOrientation in_orientation,
    const UICoord& in_gap
    )
{
    bool dirty = false;

    if (true == _component_default.SetBase(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        ))
    {
        dirty = true;
    }

    if (_orientation != in_orientation)
    {
        dirty = true;
        _orientation = in_orientation;
    }

    if (_gap != in_gap)
    {
        dirty = true;
        _gap = in_gap;
    }

    return dirty;
}

const bool UIComponentStack::SetStateFlag(const UIStateFlag in_state_flag)
{
    return _component_default.SetStateFlag(in_state_flag);
}

const bool UIComponentStack::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentStack::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentStack::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentStack::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

void UIComponentStack::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
{
    _component_default.SetUVScrollManual(in_uv_scroll, in_manual_horizontal, in_manual_vertical);
    return;
}

// Make sorting children easier
void UIComponentStack::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
}

void* UIComponentStack::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentStack::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    return _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        );
}

const bool UIComponentStack::UpdateSize(
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
    // the default _component_default.UpdateSize recurses, we do want to do that, but with an offset to where to put things
    // else the default behaviour is canvas to draw all the children over the top of each other
 
    bool dirty = false;
    if (true == _component_default.Update(in_time_delta))
    {
        dirty = true;
    }

     std::vector<VectorInt4> child_window_offset_array;

    VectorInt2 layout_size;
    VectorInt2 desired_size;
    GetStackDesiredSize(
        layout_size,
        desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        child_window_offset_array
        );

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    UIComponentDefault::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        _component_default.GetUVScroll(),
        _component_default.GetUVScrollManualX(),
        _component_default.GetUVScrollManualY(),
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta, 
        layout_size,
        desired_size,
        _component_default.GetInUseLayout()
        );

    // Update geometry
    if (true == in_out_geometry.Set(
        geometry_pos,
        geometry_uv
        ))
    {
        dirty = true;
    }

    in_out_node.UpdateTextureSize(texture_size, dirty);
    out_screen_space.Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    int trace = 0;
    auto& child_data_array = in_out_node.GetChildData();
    for (auto& child_data_ptr : child_data_array)
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;

        if (nullptr == child_data._component)
        {
            continue;
        }

        VectorInt4& child_window_offset = child_window_offset_array[trace];
        trace += 1;
        // invert y, 0,0 is bottom left
        const int height = texture_size.GetY() - (child_window_offset.GetW() + child_window_offset.GetY());
        const VectorInt2 window(child_window_offset.GetX(), child_window_offset.GetY());
        const VectorInt2 offset(child_window_offset.GetZ(), height);

        if (true == child_data._component->UpdateSize(
            in_draw_system,
            texture_size,
            offset,
            window,
            in_ui_scale,
            in_time_delta,
            *(child_data._geometry.get()),
            *(child_data._node.get()),
            out_screen_space,
            *(child_data._screen_space.get())
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

void UIComponentStack::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    std::vector<VectorInt4> child_window_offset_array;
    GetStackDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        child_window_offset_array
        );
}

void UIComponentStack::GetStackDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    std::vector<VectorInt4>& out_child_window_offset
    )
{
    out_layout_size = _component_default.GetInUseLayout().GetSize(in_parent_window, in_ui_scale);
    const int gap = _gap.Calculate(in_parent_window, in_ui_scale);

    VectorInt2 max_desired_size;
    for (auto& child_data_ptr : in_out_node.GetChildData())
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;

        if (nullptr == child_data._component)
        {
            continue;
        }

        VectorInt2 child_layout;
        VectorInt2 child_desired;
        child_data._component->GetDesiredSize(
            child_layout,
            child_desired,
            out_layout_size, 
            in_ui_scale,
            *(child_data._node)
            );
        VectorInt4 window_offset(child_desired.GetX(), child_desired.GetY(), 0, 0);
        switch(_orientation)
        {
        default:
            break;
        case UIOrientation::TVertical:
            if (0 != max_desired_size[1])
            {
                max_desired_size[1] += gap;
            }
            window_offset[3] = max_desired_size[1];
            max_desired_size[0] = std::max(max_desired_size[0], child_desired[0]);
            max_desired_size[1] += child_desired[1];

            break;
        case UIOrientation::THorizontal:
            if (0 != max_desired_size[0])
            {
                max_desired_size[0] += gap;
            }
            window_offset[2] = max_desired_size[0];
            max_desired_size[0] += child_desired[0];
            max_desired_size[1] = std::max(max_desired_size[1], child_desired[1]);

            break;
        }

        out_child_window_offset.push_back(window_offset);
    }

    out_layout_size =_component_default.GetInUseLayout().CalculateShrinkSize(out_layout_size, max_desired_size);
    out_desired_size = max_desired_size;

    for (auto& iter : out_child_window_offset)
    {
        switch(_orientation)
        {
        default:
            break;
        case UIOrientation::TVertical:
            iter[0] = out_layout_size[0];
            break;
        case UIOrientation::THorizontal:
            iter[1] = out_layout_size[1];
            break;
        }
    }

    return;
}

const bool UIComponentStack::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _component_default.PreDraw(
        in_draw_param,
        in_node
        );
}

const VectorFloat4 UIComponentStack::GetTintColour() const
{
    return _component_default.GetTintColour();
}
