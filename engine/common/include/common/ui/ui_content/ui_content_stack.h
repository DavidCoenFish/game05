#pragma once

#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_layout.h"

enum class StackOrientation
{
    TVertical,
    THorizontal,
    //TGridLeftRightTopBottom?
    //TGridTopBottomLeftRight?
    //TFlowLeftRightTopBottom
    //TFlowTopBottomLeftRight
};

class UIContentStack : public IUIContent
{
public:
    UIContentStack(
        const UILayout& in_layout,
        const StackOrientation in_orientation,
        const UICoord& in_gap,
        const bool in_shrink_to_fit
        );
    virtual ~UIContentStack();

    const bool Set(
        const UILayout& in_layout,
        const StackOrientation in_orientation,
        const UICoord& in_gap,
        const bool in_shrink_to_fit
        );

private:
    virtual const bool UpdateHierarchy(
        std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

private:
    UILayout _layout;
    StackOrientation _orientation;
    UICoord _gap;
    bool _shrink_to_fit;

};
