#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_stack.h"

#include "common/math/vector_int4.h"
#include "common/ui/ui_content/ui_content_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIContentStack::UIContentStack(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    const StackOrientation in_orientation,
    const UICoord& in_gap
    )
    : _content_default(
        in_clear_background, 
        in_clear_colour,
        in_layout
        )
    , _orientation(in_orientation)
    , _gap(in_gap)
{
    // Nop
}

UIContentStack::~UIContentStack()
{
    // Nop
}

const bool UIContentStack::Set(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    const StackOrientation in_orientation,
    const UICoord& in_gap
    )
{
    bool dirty = false;

    if (true == _content_default.SetBase(
        in_clear_background, 
        in_clear_colour,
        in_layout
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

// Make sorting children easier
void UIContentStack::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
}

void* UIContentStack::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIContentStack::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIContentStack::UpdateHierarchy(
    IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    return _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        );
}

void UIContentStack::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    // the default UpdateSize recurses, we do want to do that, but with an offset to where to put things

    const VectorInt2 layout_size = _content_default.GetLayout().GetSize(in_parent_size, in_ui_scale);
    std::vector<VectorInt4> child_desired_size_offset;

    const VectorInt2 max_desired_size = GetStackDesiredSize(
        layout_size,
        in_ui_scale,
        in_out_node,
        child_desired_size_offset
        );

    const VectorInt2 actual_initial_size = _content_default.GetLayout().CalculateShrinkSize(layout_size, max_desired_size);

    VectorFloat4 geometry_pos;
    VectorFloat4 geometry_uv;
    VectorInt2 texture_size;

    UIContentDefault::CalculateGeometry(
        geometry_pos,
        geometry_uv,
        texture_size,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta, 
        actual_initial_size,
        max_desired_size,
        _content_default.GetLayout()
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

    in_out_node.UpdateTextureSize(texture_size, dirty);
    int trace = 0;
    for (auto& child_data_ptr : in_out_node.GetChildData())
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;

        if (nullptr == child_data._content)
        {
            continue;
        }

        VectorInt4& child_desired_offset = child_desired_size_offset[trace];
        trace += 1;
        VectorInt2 window(child_desired_offset.GetX(), child_desired_offset.GetY());
        const VectorInt2 offset(child_desired_offset.GetZ(), child_desired_offset.GetW());

        switch(_orientation)
        {
        default:
            break;
        case StackOrientation::TVertical:
            window[0] = actual_initial_size[0]; //max_desired_size[0];
            break;
        case StackOrientation::THorizontal:
            window[1] = actual_initial_size[1]; //max_desired_size[1];
            break;
        }

        child_data._content->UpdateSize(
            in_draw_system,
            texture_size,
            offset,
            window,
            in_ui_scale,
            in_time_delta,
            *(child_data._geometry.get()),
            *(child_data._node.get())
            );
    }

    return;
}

const VectorInt2 UIContentStack::GetDesiredSize(
    const VectorInt2&,
    const float,
    UIHierarchyNode& // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    return VectorInt2();
}

const VectorInt2 UIContentStack::GetStackDesiredSize(
    const VectorInt2& in_initial_size,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    std::vector<VectorInt4>& out_child_desired_size_offset
    )
{
    //const VectorInt2 layout_desired_size = _content_default.GetLayout().GetSize(in_parent_size, in_ui_scale);
    //const VectorInt2 layout_desired_size = _content_default.GetDesiredSize(
    //    in_parent_size,
    //    in_ui_scale,
    //    in_out_node
    //    );

    const int gap = _gap.Calculate(in_initial_size, in_ui_scale);
    VectorInt2 children_stack;
    for (auto& child_data_ptr : in_out_node.GetChildData())
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;

        if (nullptr == child_data._content)
        {
            continue;
        }

        const VectorInt2 child_desired = child_data._content->GetDesiredSize(
            in_initial_size, 
            in_ui_scale,
            *(child_data._node)
            );
        VectorInt4 desired_size_offset(child_desired.GetX(), child_desired.GetY(), 0, 0);
        switch(_orientation)
        {
        default:
            break;
        case StackOrientation::TVertical:
            if (0 != children_stack[1])
            {
                children_stack[1] += gap;
            }
            desired_size_offset[3] = children_stack[1];
            children_stack[0] = std::max(children_stack[0], std::min(child_desired[0], in_initial_size[0]));
            children_stack[1] += child_desired[1];
            break;
        case StackOrientation::THorizontal:
            if (0 != children_stack[0])
            {
                children_stack[0] += gap;
            }
            desired_size_offset[2] = children_stack[0];
            children_stack[0] += child_desired[0];
            children_stack[1] = std::max(children_stack[1], std::min(child_desired[1], in_initial_size[1]));
            break;
        }

        out_child_desired_size_offset.push_back(desired_size_offset);
    }

    return children_stack;
}
