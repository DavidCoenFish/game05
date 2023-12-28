#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_stack.h"

#include "common/math/vector_int4.h"

#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_hierarchy_node_child.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_enum.h"

UIComponentStack::UIComponentStack(
    const UIOrientation in_orientation,
    const UICoord& in_gap
    )
    : IUIComponent()
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
    const UIOrientation in_orientation,
    const UICoord& in_gap
    )
{
    bool dirty = false;

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

const VectorInt2 UIComponentStack::GetDesiredSize(
    UIHierarchyNodeChild& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_window
    )
{
    VectorInt2 base_layout_size = in_component_owner.GetLayout().GetLayoutSize(in_parent_window, in_param._ui_scale);
    std::vector<VectorInt4> child_window_offset_array;
    const VectorInt2 base_desired_size = CalculateDesiredSize(
        in_component_owner, 
        in_param, 
        base_layout_size,
        child_window_offset_array
        );

    return base_desired_size;
}

void UIComponentStack::UpdateLayout(
    UIHierarchyNodeChild& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset
    )
{
    // calculate layout size given parent window
    VectorInt2 base_layout_size = in_component_owner.GetLayout().GetLayoutSize(in_parent_window, in_param._ui_scale);
    const VectorInt4& texture_margin = in_component_owner.GetLayout().GetTextureMarginRef();

    std::vector<VectorInt4> child_window_offset_array;
    const VectorInt2 base_desired_size = CalculateDesiredSize(
        in_component_owner, 
        in_param, 
        base_layout_size,
        child_window_offset_array
        );

    in_component_owner.Finalise(
        base_layout_size,
        base_desired_size,
        in_parent_offset
        );

    int trace = 0;
    auto& child_data_array = in_component_owner.GetNode().GetChildData();
    const VectorInt2 texture_size = in_component_owner.GetNode().GetTextureSize(in_param._draw_system);
    for (auto& child_data_ptr : child_data_array)
    {
        UIHierarchyNodeChild& child_data = *child_data_ptr;
        VectorInt4& child_window_offset = child_window_offset_array[trace];
        trace += 1;

        // invert y, 0,0 is bottom left. by default, this pushes the stack children into the top left of layout
        // todo: the stack could have enum of alignment to left/top/bottom and ajust the offset here?
        const int height = texture_size.GetY();
        const VectorInt2 window(child_window_offset.GetX(), child_window_offset.GetY());
        const VectorInt2 offset(
            child_window_offset.GetZ() + texture_margin[0],
            height - (child_window_offset.GetY() + child_window_offset.GetW() + texture_margin[1])
            );

        child_data.UpdateLayout(
            in_param,
            window,
            offset
            );
    }
}

const VectorInt2 UIComponentStack::CalculateDesiredSize(
    UIHierarchyNodeChild& in_component_owner,
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_base_layout_size,
    std::vector<VectorInt4>& out_child_window_offset_array
    )
{
    const int gap = _gap.Calculate(in_base_layout_size, in_param._ui_scale);

    VectorInt2 max_desired_size;
    for (auto& child_data_ptr : in_component_owner.GetNode().GetChildData())
    {
        UIHierarchyNodeChild& child_data = *child_data_ptr;

        // how to skip null components?  
        VectorInt2 child_desired = child_data.GetDesiredSize(
            in_param,
            in_base_layout_size
            );

        VectorInt4 window_offset(child_desired.GetX(), child_desired.GetY(), 0, 0);
        switch(_orientation)
        {
        default:
            break;
        case UIOrientation::TVertical:
            if ((0 != max_desired_size[1]) && (0 != child_desired[1]))
            {
                max_desired_size[1] += gap;
            }
            window_offset[3] = max_desired_size[1];
            max_desired_size[0] = std::max(max_desired_size[0], child_desired[0]);
            max_desired_size[1] += child_desired[1];

            break;
        case UIOrientation::THorizontal:
            if ((0 != max_desired_size[0]) && (0 != child_desired[0]))
            {
                max_desired_size[0] += gap;
            }
            window_offset[2] = max_desired_size[0];
            max_desired_size[0] += child_desired[0];
            max_desired_size[1] = std::max(max_desired_size[1], child_desired[1]);

            break;
        }

        out_child_window_offset_array.push_back(window_offset);
    }

    for (auto& iter : out_child_window_offset_array)
    {
        switch(_orientation)
        {
        default:
            break;
        case UIOrientation::TVertical:
            iter[0] = max_desired_size[0];
            break;
        case UIOrientation::THorizontal:
            iter[1] = max_desired_size[1];
            break;
        }
    }

    max_desired_size = in_component_owner.GetLayout().ApplyMargin(
        max_desired_size,
        in_param._ui_scale
        );

    return max_desired_size;
}

