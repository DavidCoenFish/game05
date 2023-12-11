#pragma once

#include "common/ui/ui_component/i_ui_component.h"

class UILayout;

/// Currently the default behaviour of IUIComponent is to act as a canvas
/// each child is drawn to the parent at the given layout
/// result is that UIComponentCanvas could be a typedef of IUIComponent
class UIComponentCanvas : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentCanvas(
        const UILayout& in_layout,
        void* in_source_token = nullptr
        );
    virtual ~UIComponentCanvas();

private:
    virtual const VectorInt2 GetDesiredSize(
        UIHierarchyNodeChildData& in_component_owner,
        const UIHierarchyNodeUpdateParam& in_layout_param,
        const VectorInt2& in_pre_shrink_layout_size //in_parent_window
        ) override;

};
