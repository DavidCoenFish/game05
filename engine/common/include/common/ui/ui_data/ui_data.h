#pragma once

class UIData
{
public:
    UIData(
        const std::string& in_template_name = std::string("UIData"),
        const std::vector<std::shared_ptr<UIData>>& in_array_child_data = std::vector<std::shared_ptr<UIData>>()
        );
    virtual ~UIData();

    const bool VisitDataArray(const std::function<bool(const std::vector<std::shared_ptr<UIData>>&)>& in_visitor) const;
    std::vector<std::shared_ptr<UIData>>& ModifyData();

    const std::string& GetTemplateName() const;

private:
    std::string _template_name;
    std::vector<std::shared_ptr<UIData>> _array_child_data;

};
