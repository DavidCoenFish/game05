#pragma once

class IUIData;

class IUIModel
{
public:
    virtual ~IUIModel();

    virtual const IUIData* const GetData(
        const std::string& in_key
        ) const = 0;

};
