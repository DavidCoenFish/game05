#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_combo_box_layer.h"

UIDataComboBoxLayer::UIDataComboBoxLayer(
    const TOnActivate& in_on_activate,
    const bool in_active,
    const std::shared_ptr<UIData>& in_insert_dropdown_parent,
    const std::string& in_template_name,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data
    )
    : UIData(
        in_template_name,
        in_array_child_data
        )
    , _on_activate(in_on_activate)
    , _active(in_active)
    , _insert_dropdown_parent(in_insert_dropdown_parent)
{
    // Nop
}

UIDataComboBoxLayer::~UIDataComboBoxLayer()
{
    // Nop
}
