#pragma once

class IUIData
{
public:
    virtual ~IUIData();

    virtual const std::string GetTemplateName() const = 0;
    //virtual const int GetChangeID() const;

    // This could work, but doesn't allow multiple templates per ui data class, ie, UIDataTemplate, which may be wanted for layout flexibility
    //virtual BuildChildren(
    //    UIManager* const in_ui_manager,
    //    UIHierarchyNode& in_parent_node, // call ClearChildren, AddChild
    //    const IUIProviderData* const in_provider_data,
    //    const std::string& in_provider_data_key_base,
    //    DrawSystem* const in_draw_system,
    //    ID3D12GraphicsCommandList* const in_command_list
    //    );

    //virtual const std::string GetTemplateName() const = 0;
    //virtual const std::string GetProviderDataKeyBase() const = 0;

};
