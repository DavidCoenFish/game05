#pragma once

#include "common/ui/ui_component/i_ui_component.h"

/// Currently the default behaviour of IUIComponent is to act as a canvas
/// each child is drawn to the parent at the given layout
/// result is that UIComponentCanvas could be a typedef of IUIComponent
class UIComponentCanvas : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentCanvas();
    virtual ~UIComponentCanvas();

};
