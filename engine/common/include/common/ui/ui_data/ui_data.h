#pragma once

/// base ui data class ends up representing minimal data for ui hierarchy
/// moving towards only run time modified data (like a string) to be in a class derrived from UIData
/// the rest of the data, like effect type or size param, are moved into construction factories
/// see UIManager::AddContentFactory
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
