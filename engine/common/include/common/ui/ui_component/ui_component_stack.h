#pragma once

#include "common/ui/ui_component/i_ui_component.h"

class VectorInt4;
enum class UIOrientation;

class UIComponentStack : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentStack(
        const UIOrientation in_orientation,
        const UICoord& in_gap
        );
    virtual ~UIComponentStack();

    const bool Set(
        const UIOrientation in_orientation,
        const UICoord& in_gap
        );

private:
    virtual const VectorInt2 GetDesiredSize(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_layout_param,
        const VectorInt2& in_parent_window
        ) override;

    virtual void UpdateLayout(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_parent_window
        ) override;

    const VectorInt2 CalculateDesiredSize(
        UIHierarchyNodeChild& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_base_layout_size_minus_margin,
        const VectorInt2& in_base_offset,
        std::vector<VectorInt4>& out_child_window_offset_array
        );

private:
    UIOrientation _orientation;
    UICoord _gap;

};
