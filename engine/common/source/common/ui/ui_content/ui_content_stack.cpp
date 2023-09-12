#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"

UIContentStack::UIContentStack(
    const UILayout& in_layout,
    const StackOrientation in_orientation,
    const UICoord& in_gap,
    const bool in_shrink_to_fit
    )
    : _layout(in_layout)
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
    const UILayout& in_layout,
    const StackOrientation in_orientation,
    const UICoord& in_gap,
    const bool in_shrink_to_fit
    )
{
    bool dirty = false;
    if (_layout != in_layout)
    {
        dirty = true;
        _layout = in_layout;
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

const bool UIContentStack::UpdateHierarchy(
    std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
    const IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
}
