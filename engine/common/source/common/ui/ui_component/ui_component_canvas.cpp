#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIComponentCanvas::UIComponentCanvas(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
    : _component_default(
        in_base_colour,
        in_layout
        )
{
    // Nop
}

UIComponentCanvas::~UIComponentCanvas()
{
    // Nop
}

const bool UIComponentCanvas::Set(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
{
    return _component_default.SetBase(
        in_base_colour,
        in_layout
        );
}

const bool UIComponentCanvas::SetStateFlag(const UIStateFlag in_state_flag)
{
    return _component_default.SetStateFlag(in_state_flag);
}

const UIStateFlag UIComponentCanvas::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentCanvas::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentCanvas::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentCanvas::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentCanvas::UpdateHierarchy(
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

void UIComponentCanvas::UpdateSize(
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
    _component_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node,
        in_parent_screen_space,
        out_screen_space,
        in_layout_override
        );
}

void UIComponentCanvas::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    UILayout* const in_layout_override
    )
{
    return _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node,
        in_layout_override
        );
}

const bool UIComponentCanvas::Draw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _component_default.Draw(
        in_draw_param,
        in_node
        );
}
