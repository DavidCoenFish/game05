#pragma once

class UIData;

/// What is currentl missing from IUIModel/UIData is a mechanisim to mark heierarchy dirty and only recaculate hierarchy as needed
/// Then resolving the hierarchy to set size dirty, and only resolve size when needed
/// Needing to render is expressed by UITexture::[has drawn, always draw] flags...
class IUIModel
{
public:
    virtual ~IUIModel();

    virtual const bool VisitDataArray(
        const std::string& in_key,
        const std::function<void(const std::vector<std::shared_ptr<UIData>>&)>& in_visitor
        ) const = 0;

    virtual UIData* const GetData(
        const std::string& in_key
        ) const = 0;

    template <class InType>
    InType* const GetDataType(
        const std::string& in_key
        )
    {
        UIData* const data = GetData(in_key);
        return dynamic_cast<InType*>(data);
    }

    // make a visitor, or provide tools to set locale on string/textrun resources? (or both)
};
