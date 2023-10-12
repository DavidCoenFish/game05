#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_stack.h"

#include "common/math/vector_int4.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIComponentStack::UIComponentStack(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const StackOrientation in_orientation,
    const UICoord& in_gap
    )
    : _content_default(
        in_base_colour,
        in_layout
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
    const StackOrientation in_orientation,
    const UICoord& in_gap
    )
{
    bool dirty = false;

    if (true == _content_default.SetBase(
        in_base_colour,
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
void UIComponentStack::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
}

void* UIComponentStack::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

const bool UIComponentStack::SetLayout(const UILayout& in_layout)
{
    return _content_default.SetLayout(in_layout);
}

const bool UIComponentStack::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;

    // currently UIDataContainer is handled by _content_default.UpdateHierarchy

    if (true == _content_default.UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        ))
    {
        dirty = true;
    }

    return dirty;
}

void UIComponentStack::UpdateSize(
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
    // the default UpdateSize recurses, we do want to do that, but with an offset to where to put things

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
        _content_default.GetUVScroll(),
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale,
        in_time_delta, 
        layout_size,
        desired_size,
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
    out_screen_space.Update(
        in_parent_screen_space,
        geometry_pos,
        geometry_uv
        );

    int trace = 0;
    for (auto& child_data_ptr : in_out_node.GetChildData())
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;

        if (nullptr == child_data._component)
        {
            continue;
        }

        VectorInt4& child_window_offset = child_window_offset_array[trace];
        trace += 1;
        VectorInt2 window(child_window_offset.GetX(), child_window_offset.GetY());
        const VectorInt2 offset(child_window_offset.GetZ(), child_window_offset.GetW());

        child_data._component->UpdateSize(
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
            );
    }

    return;
}

void UIComponentStack::GetDesiredSize(
    VectorInt2&, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2&, // if bigger than layout size, we need to scroll
    const VectorInt2&,
    const float,
    UIHierarchyNode& // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    return;
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
    out_layout_size = _content_default.GetLayout().GetSize(in_parent_window, in_ui_scale);
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
        case StackOrientation::TVertical:
            if (0 != max_desired_size[1])
            {
                max_desired_size[1] += gap;
            }
            window_offset[3] = max_desired_size[1];
            max_desired_size[0] = std::max(max_desired_size[0], child_desired[0]);
            max_desired_size[1] += child_desired[1];

            break;
        case StackOrientation::THorizontal:
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

    out_layout_size = _content_default.GetLayout().CalculateShrinkSize(out_layout_size, max_desired_size);
    out_desired_size = max_desired_size;

    for (auto& iter : out_child_window_offset)
    {
        switch(_orientation)
        {
        default:
            break;
        case StackOrientation::TVertical:
            iter[0] = out_layout_size[0];
            break;
        case StackOrientation::THorizontal:
            iter[1] = out_layout_size[1];
            break;
        }
    }

    return;
}

const bool UIComponentStack::Draw(
    const UIManagerDrawParam& in_draw_param,
    Shader* const in_shader,
    UIHierarchyNode& in_node
    ) 
{
    return _content_default.Draw(
        in_draw_param,
        in_shader,
        in_node
        );
}
