#pragma once

/// base ui data class ends up representing minimal data for ui hierarchy
/// moving towards only run time modified data (like a string) to be in a class derrived from UIData
/// the rest of the data, like effect type or size param, are moved into construction factories
/// see UIManager::AddContentFactory
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

    // const bool GetParentSet()
    // void SetParent(UIData* const)
    // const bool GetHierarchyDirty
    // void SetHierarchyDirty(const bool in_dirty)
    //// const bool GetLayoutDirty
    //// void SetLayoutDirty(const bool in_dirty)

    // Render dirty is expressed by UITexture [_has_drawn, _allyas_draw], or have a render dirty here as well when data effecting render changes? or is that done in layout
    

private:
    std::string _template_name;
    std::vector<std::shared_ptr<UIData>> _array_child_data;

    //std::weak_ptr<UIData> _parent;
    //bool _hierarchy_dirty;
    //bool _layout_dirty; // and or what size data was input. note, data is only used for hierarchy? or in size as well? think it is in use for size
    // move to not reference data in UpdateSize?

};
