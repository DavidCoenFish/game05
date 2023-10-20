#pragma once

#include "common/ui/ui_data/ui_data.h"

class VectorFloat2;

class UIDataGrid : public UIData
{
public:
    UIDataGrid(
        const std::string& in_template_name = std::string("UIDataButton"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIDataGrid();

    const bool GetEnabled() const { return _enabled; }
    const std::function<void(const VectorFloat2&)>& GetOnClick() const { return _on_click; }

private:
    bool _enabled;
    std::function<void(const VectorFloat2&)> _on_click;

};
