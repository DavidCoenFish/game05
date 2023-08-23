#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class UIDataContainer : public IUIData
{
public:
    UIDataContainer(
        const std::vector<std::shared_ptr<IUIData>>& in_array_data,
        const std::string& in_template_name = std::string("UIDataContainer")
        );
    virtual ~UIDataContainer();

    const std::vector<std::shared_ptr<IUIData>>& GetDataConst() const { return _array_data; }
    std::vector<std::shared_ptr<IUIData>>& ModifyData();

private:
    virtual const std::string GetTemplateName() const { return _template_name; }
    virtual const int GetChangeID() const override;

private:
    std::string _template_name;
    int _change_id;
    std::vector<std::shared_ptr<IUIData>> _array_data;

};
