#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_stack.h"

#include "common/math/vector_int4.h"

#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_enum.h"

UIComponentStack::UIComponentStack(
    const UILayout& in_layout,
    void* in_source_token,
    const UIOrientation in_orientation,
    const UICoord& in_gap
    )
    : IUIComponent(
        in_layout,
        in_source_token
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
    const UILayout& in_layout,
    const UIOrientation in_orientation,
    const UICoord& in_gap
    )
{
    bool dirty = false;

    SetLayout(in_layout);

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

#if 0 //OLD_CODE
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
    if (true == TSuper::Update(in_time_delta))
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
    out_layout_size = GetLayout().GetSize(in_parent_window, in_ui_scale);
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

    out_layout_size = GetLayout().CalculateShrinkSize(out_layout_size, max_desired_size);
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
#endif