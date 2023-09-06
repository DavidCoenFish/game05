#pragma once

class IUIData;

class IUIModel
{
public:
    virtual ~IUIModel();

    virtual const IUIData* const GetData(
        const std::string& in_key
        ) const = 0;

    virtual void VisitDataArray(
        const std::string& in_key,
        const std::function<bool(const std::vector<std::shared_ptr<IUIData>>&)>& in_visitor
        ) const = 0;

};
