#pragma once

#include "common\ui\ui_layout.h"
#include "common\ui\ui_base_colour.h"
#include "common\ui\ui_enum.h"

class IUIComponent;
class UIHierarchyNode;
class UIScreenSpace;
struct UIHierarchyNodeChildData;
struct UIHierarchyNodeUpdateHierarchyParam;
struct UIHierarchyNodeUpdateLayoutRenderParam;

/// base ui data class ends up representing minimal data for ui hierarchy
/// moving towards only run time modified data (like a string) to be in a class derrived from UIData
/// the rest of the data, like effect type or size param, are moved into construction factories
/// see UIManager::AddContentFactory
/// moving away from construction factories, and putting stuff back into the model. 
/// if layout now directly in model, should UIComponentSlider still override/ or change the layout in the model
/// Why not have UIDataSlider modify UIData layout? or was that the domain of UIContentSlider. 
class UIData
{
public:
    typedef std::array<VectorFloat4, static_cast<int>(UIStateFlag::TTintPermutationCount)> TStateFlagTintArray;

    UIData(
        const UILayout& in_layout,
        const UIBaseColour& in_base_colour,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null,
        UIData* const in_parent_or_null = nullptr
        );
    virtual ~UIData();

    // return reference to child, to allow chain
    UIData* const AddChild(const std::shared_ptr<UIData>& in_data_child);
    UIData* const SetChild(const std::shared_ptr<UIData>& in_data_child, const int in_index);
    void ClearAllChildren(void);

    /// return true if any bit of flag is set
    const bool GetDirtyBit(const UIDataDirty in_flag) const;

    /// modifying hierarchy [AddChild, SetChild, ClearAllChildren] automatically sets hierarchy modified flag, 
    /// otherwise client code is expexed to set dirty flag appropriatly, ie, if a UIData derrive class changes something that effects render
    void SetDirtyBit(const UIDataDirty in_flag, const bool in_active);
    
    /// visitor rather than accessor for child data, but there otherways of delaing with threadding protection, like making a queue for UIModel operations and only performing them on one thread
    //const bool VisitDataArray(const std::function<bool(const std::vector<std::shared_ptr<UIData>>&)>& in_visitor) const;
    const std::vector<std::shared_ptr<UIData>>& GetArrayChildData() const { return _array_child_data; }
    //
    const UILayout& GetLayout() const { return _layout; }
    const UIBaseColour& GetBaseColour() const { return _base_colour; }
    const TStateFlagTintArray* const GetStateFlagTintArray() const { return _state_flag_tint_array_or_null.get(); }

    /// Make component type match what the data wants, default is UIComponentCanvas
    virtual void UpdateHierarchy(
        std::unique_ptr<IUIComponent>& in_out_component,
        const UIHierarchyNodeUpdateHierarchyParam& in_param,
        const int in_index = 0
        );

    /// replacing what was done in the factory/ component
    virtual void UpdateLayoutRender(
        IUIComponent& in_component,
        UIHierarchyNodeChildData& in_component_owner,
        const VectorInt2& in_texture_size,
        const UIHierarchyNodeUpdateLayoutRenderParam& in_param,
        const UIScreenSpace& in_parent_screen_space
        );

private:
    /// don't have a copy of UILayout in the IUIComponent anymore, IUIComponent has UIGeometry though
    /// modification of layout should invalidate the Layout flag
    UILayout _layout;
    /// modification of basecolour should invalidate the Layout flag
    UIBaseColour _base_colour;
    /// modification of tint array should invalidate the render flag
    std::shared_ptr<const TStateFlagTintArray> _state_flag_tint_array_or_null;
    /// parent can be null if we are the root node
    UIData* _parent_or_null;
    /// to help speedup UI update, have a change tree, if a child has a dirty flag set, also set parent
    UIDataDirty _dirty_flag;
    /// children array, currently no recusion protection
    std::vector<std::shared_ptr<UIData>> _array_child_data;

};
