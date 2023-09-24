#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"

UIContentStack::UIContentStack(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    const StackOrientation in_orientation,
    const UICoord& in_gap,
    const bool in_shrink_to_fit
    )
    : _content_default(
        in_clear_background, 
        in_clear_colour,
        in_layout
        )
    , _orientation(in_orientation)
    , _gap(in_gap)
    , _shrink_to_fit(in_shrink_to_fit)
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
    const UICoord& in_gap,
    const bool in_shrink_to_fit
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

    if (_shrink_to_fit != in_shrink_to_fit)
    {
        dirty = true;
        _shrink_to_fit = in_shrink_to_fit;
    }

    return dirty;
}

//const bool UIContentStack::GetClearBackground(
//    VectorFloat4& out_clear_colour
//    ) const
//{
//    if (_clear_background)
//    {
//        out_clear_colour = _clear_colour;
//    }
//    return _clear_background;
//}

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
    //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
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
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _content_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_ui_scale,
        in_time_delta,
        in_out_geometry,
        in_out_node
        );
}

const VectorInt2 UIContentStack::GetDesiredSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    return _content_default.GetDesiredSize(
        in_parent_size,
        in_ui_scale,
        in_out_node
        );
}
