#pragma once

#include "common/ui/ui_content/i_ui_content.h"
#include "common/ui/ui_layout.h"

class UIContentCanvas : public IUIContent
{
public:
    UIContentCanvas(
        const UILayout& in_layout
        );
    virtual ~UIContentCanvas();

private:
    virtual const bool UpdateHierarchy(
        std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

private:
    UILayout _layout;

};
