#pragma once

/// base ui data class ends up representing minimal data for ui hoerarchy
/// moving towards only run time modified deat (like a string) to be in a derrived ui data class
/// the rest of the data, like effect type or size param, are moved into construction factories
/// UIManager::AddContentFactory(
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
