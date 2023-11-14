#pragma once

#include "common/ui/ui_data/ui_data.h"

/// a menu tree may have multiple drop downs? active as int count? deal with combo box first
class UIDataComboBoxLayer : public UIData
{
public:
    typedef std::function<void()> TOnActivate;

    explicit UIDataComboBoxLayer(
        const TOnActivate& in_on_activate = nullptr,
        const bool in_active = false,
        const std::shared_ptr<UIData>& in_insert_dropdown_parent = nullptr,
        const std::string& in_template_name = std::string("UIDataComboBoxLayer"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataComboBoxLayer();

    const bool GetActive() const { return _active; } 
    void SetActive(const bool in_active) { _active = in_active; return; } 
    UIData* const GetInsertDropdownParent() const { return _insert_dropdown_parent.get(); } 

    const TOnActivate GetOnActivate() const { return _on_activate; }

private:
    /// allow the document layer to comunicate with the model that a dropdown has been activated
    TOnActivate _on_activate;

    /// reflect in the model if a combo box dropdown is active
    bool _active;

    /// identify the parent of node where to insert the dropdown into
    std::shared_ptr<UIData> _insert_dropdown_parent;

};
