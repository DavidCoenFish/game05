#pragma once

class IUIData;

class IUIModel
{
public:
    virtual ~IUIModel();

    virtual const bool VisitDataArray(
        const std::string& in_key,
        const std::function<void(const std::vector<std::shared_ptr<IUIData>>&)>& in_visitor
        ) const = 0;

};
