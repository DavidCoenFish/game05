#pragma once

#include "common/ui/ui_component/i_ui_component.h"

class VectorInt4;
enum class UIOrientation;

class UIComponentStack : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentStack(
        const UILayout& in_layout,
        void* in_source_token,
        const UIOrientation in_orientation,
        const UICoord& in_gap
        );
    virtual ~UIComponentStack();

    const bool Set(
        const UILayout& in_layout,
        const UIOrientation in_orientation,
        const UICoord& in_gap
        );

private:
    virtual const VectorInt2 GetDesiredSize(
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_layout_param,
        const VectorInt2& in_parent_window
        ) override;

    virtual void UpdateLayout(
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_parent_window
        ) override;

    virtual void UpdateResources(
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_param,
        const UIScreenSpace& in_parent_screen_space,
        const VectorInt2& in_parent_texture_size
        ) override;

private:
    UIOrientation _orientation;
    UICoord _gap;

};
