#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_button.h"
#include "common/ui/ui_data/ui_data_button.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"

UIComponentButton::UIComponentButton(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
    : _content_default(
        in_base_colour,
        in_layout
        )
{
    // Nop
}

UIComponentButton::~UIComponentButton()
{
    // Nop
}

const bool UIComponentButton::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout
    )
{
    return _content_default.SetBase(
        in_base_colour,
        in_layout
        );
}

const bool UIComponentButton::Set(
    const bool in_enabled,
    const std::function<void(const VectorFloat2&)>& in_on_click
    )
{
    bool dirty = false;
    if (_enabled != in_enabled)
    {
        dirty = true;
        _enabled = in_enabled;
    }

    _on_click = in_on_click;

    //if (_on_click != in_on_click)
    //{
    //    _on_click = in_on_click;
    //}

    return dirty;
}

void UIComponentButton::SetSourceToken(void* in_source_ui_data_token)
{
    _content_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentButton::GetSourceToken() const
{
    return _content_default.GetSourceToken();
}

//const bool UIComponentButton::SetLayout(const UILayout& in_layout)
//{
//    return _content_default.SetLayout(in_layout);
//}

const bool UIComponentButton::UpdateHierarchy(
    //std::vector<std::shared_ptr<UIData>>*& out_array_data_or_null,
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    const UIDataButton* const data = dynamic_cast<const UIDataButton*>(in_data);
    if (nullptr != data)
    {
        if (true == Set(
            data->GetEnabled(),
            data->GetOnClick()
            ))
        {
            dirty = true;
        }
    }

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

void UIComponentButton::UpdateSize(
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
    _content_default.UpdateSize(
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
        out_screen_space
        );
}

void UIComponentButton::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    return _content_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );
}

void UIComponentButton::OnInputMouseClick(
    const VectorFloat4&, // in_screen_pos,
    const VectorFloat2& in_mouse_pos
    )
{
    if (nullptr != _on_click)
    {
        _on_click(in_mouse_pos);
    }
    return;
}

const bool UIComponentButton::Draw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _content_default.Draw(
        in_draw_param,
        in_node
        );
}

