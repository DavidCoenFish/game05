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

    virtual IUIData* const GetData(
        const std::string& in_key
        ) const = 0;

    template <class InType>
    InType* const GetDataType(
        const std::string& in_key
        )
    {
        IUIData* const data = GetData(in_key);
        return dynamic_cast<InType*>(data);
    }
};
