#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_coord.h"

struct UIHierarchyNodeUpdateParam;

class UIDataStack : public UIData
{
    typedef UIData TSuper;
public:
    /// let the text manager what locale hint to render text as, for harfbuz
    UIDataStack(
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const UITintColour& in_tint_colour,
        const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data = s_empty_effect_data_array,
        const UIOrientation in_orientation = UIOrientation::TVertical,
        const UICoord& in_gap = UICoord()
        );
    virtual ~UIDataStack();

private:
    virtual void ApplyComponent(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateParam& in_param,
        const int in_index = 0
        ) override;

    ///// Make component type match what the data wants, default is UIComponentCanvas
    //virtual void UpdateHierarchy(
    //    std::unique_ptr<IUIComponent>& in_out_component,
    //    const UIHierarchyNodeUpdateHierarchyParam& in_param,
    //    const int in_index = 0
    //    ) override;

    //virtual void UpdateLayoutRender(
    //    IUIComponent& in_component,
    //    UIHierarchyNodeChildData& in_component_owner,
    //    const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
    //    const VectorInt2& in_parent_size,
    //    const VectorInt2& in_parent_offset,
    //    const VectorInt2& in_parent_window,
    //    const UIScreenSpace& in_parent_screen_space
    //    ) override;

    ///// GetChild desired size? layout is now part of UIData?
    //virtual const VectorInt2 GetContentSize(
    //    IUIComponent& in_component,
    //    const VectorInt2& in_target_size, 
    //    const float in_ui_scale,
    //    UIHierarchyNodeChildData& in_component_owner
    //    ) override;

    //virtual const VectorInt2 GetDesiredSize(
    //    IUIComponent& in_component,
    //    const VectorInt2& in_target_size, 
    //    const float in_ui_scale,
    //    UIHierarchyNodeChildData& in_component_owner
    //    ) override;

private:
    UIOrientation _orientation;
    UICoord _gap;

};
