#pragma once

#include "common/ui/ui_component/i_ui_component.h"

class UIlayout;

/// Currently the default behaviour of IUIComponent is to act as a canvas
/// each child is drawn to the parent at the given layout
/// result is that UIComponentCanvas could be a typedef of IUIComponent
class UIComponentCanvas : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    UIComponentCanvas(
        const UIlayout& in_layout,
        void* in_source_token = nullptr
        );
    virtual ~UIComponentCanvas();

    void ApplyData(
        const UIlayout& in_layout
        );

};
