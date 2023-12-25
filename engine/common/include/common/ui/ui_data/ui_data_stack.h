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
#ifdef _DEBUG
        const std::string& in_debug_name,
#endif
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const UITintColour& in_tint_colour,
        const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data = s_empty_effect_data_array,
        const UIOrientation in_orientation = UIOrientation::TVertical,
        const UICoord& in_gap = UICoord()
        );
    virtual ~UIDataStack();

private:
    virtual const bool ApplyComponent(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateParam& in_param
        ) override;

private:
    UIOrientation _orientation;
    UICoord _gap;

};
