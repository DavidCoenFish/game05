#pragma once

class IUIData;

class IUIProviderData
{
public:
    virtual ~IUIProviderData();

    struct TemplateData
    {
        explicit TemplateData(
            const std::string& in_template_name = std::string(),
            const std::string& in_data_provider_root = std::string()
            )
            : _template_name(in_template_name)
            , _data_provider_root(in_data_provider_root)
        {
            // Nop
        }
        std::string _template_name;
        std::string _data_provider_root;
    };

    //virtual std::shared_ptr<IUIData> GetData(
    virtual const IUIData* const GetData(
        const std::string& in_key
        ) const = 0;

    virtual const TemplateData GetRootPage() const = 0;

    virtual const std::vector<TemplateData> GetPageTemplateArray(
        const std::string& in_page,
        const std::string& in_data_provider_root
        ) const = 0;

};
