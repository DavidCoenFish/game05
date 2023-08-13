#pragma once

class IUIData
{
public:
    virtual ~IUIData();

    virtual const std::string GetClassName() const = 0;

    //virtual const std::string GetTemplateName() const = 0;
    //virtual const std::string GetProviderDataKeyBase() const = 0;

};
