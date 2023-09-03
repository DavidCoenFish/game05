#pragma once

#include "common/ui/ui_content/ui_content_default.h"

class UIContentCanvas : public UIContentDefault
{
public:
    UIContentCanvas();
    virtual ~UIContentCanvas();

private:
    virtual const int GetClassTypeID() const override;

    //virtual const bool Update(
    //    VectorInt2& out_texture_size,
    //    UIHierarchyNodeLayoutData& out_layout_data,
    //    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
    //    const VectorInt2& in_parent_size,
    //    const IUIData* const in_data,
    //    const std::string& in_model_key,
    //    const UIHierarchyNodeUpdateLayoutParam& in_param
    //    ) override;
};
