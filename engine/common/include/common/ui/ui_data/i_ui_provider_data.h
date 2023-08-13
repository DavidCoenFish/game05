#pragma once

class IUIData;

class IUIProviderData
{
public:
    virtual ~IUIProviderData();

    virtual std::shared_ptr<IUIData> GetData(
        const std::string& in_key
        ) const = 0;

    virtual const std::string GetRootPage() const = 0;
    virtual const std::vector<std::string> GetPageTemplateArray(
        const std::string& in_page
        ) const = 0;

};
