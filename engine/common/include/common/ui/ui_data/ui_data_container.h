#pragma once

#include "common/ui/ui_data/i_ui_data.h"

class UIDataContainer : public IUIData
{
public:
    UIDataContainer(
        const std::vector<std::shared_ptr<IUIData>>& in_array_data
        );
    virtual ~UIDataContainer();

    const std::vector<std::shared_ptr<IUIData>>& GetDataConst() const { return _array_data; }
    std::vector<std::shared_ptr<IUIData>>& ModifyData();

private:
    virtual const std::string GetTemplateName() const { return "UIDataContainer"; }
    virtual const int GetChangeID() const override;

private:
    int _change_id;
    std::vector<std::shared_ptr<IUIData>> _array_data;

};
