#pragma once

#include "common/ui/ui_component/i_ui_component.h"

/// Currently the default behaviour of IUIComponent is to act as a canvas
/// each child is drawn to the parent at the given layout
/// result is that UIComponentCanvas could be a typedef of IUIComponent
class UIComponentCanvas : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentCanvas(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
        );
    virtual ~UIComponentCanvas();

};
