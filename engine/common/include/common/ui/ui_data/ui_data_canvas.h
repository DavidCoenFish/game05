#pragma once

#include "common\ui\ui_data\ui_data.h"

class UIDataCanvas : public UIData
{
private:
    /// Make component type match what the data wants, default is UIComponentCanvas
    /// return true if data has changed and that we need to renderer
    virtual const bool ApplyComponent(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateParam& in_param,
        const int in_index = 0
        ) override;

};
