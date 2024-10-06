#pragma once

#include "common\ui\ui_layout.h"
#include "common\ui\ui_base_colour.h"
#include "common\ui\ui_tint_colour.h"
#include "common\ui\ui_enum.h"

class IUIComponent;
class UIEffectComponent;
class UIEffectData;
class UIHierarchyNode;
class UIScreenSpace;
class UIHierarchyNodeChild;
struct UIHierarchyNodeUpdateParam;

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
	static const std::vector<std::shared_ptr<UIEffectData>> s_empty_effect_data_array;

	explicit UIData(
#ifdef _DEBUG
		const std::string& in_debug_name = "ui_data_root",
#endif
		const UILayout& in_layout = UILayout::FactoryFull(),
		const UIBaseColour& in_base_colour = UIBaseColour::FactoryDefault(),
		const UITintColour& in_tint_colour = UITintColour::FactoryDefault(),
		const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data = s_empty_effect_data_array,
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
	const std::vector<std::shared_ptr<UIEffectData>>& GetArrayEffectData() const { return _array_effect_data; }

	const UILayout& GetLayout() const { return _layout; }
	const UIBaseColour& GetBaseColour() const { return _base_colour; }
	const UITintColour& GetTintColour() const { return _tint_colour; }

	/// Make component type match what the data wants, default is UIComponentCanvas
	/// return true if data has changed and that we need to renderer
	virtual const bool ApplyComponent(
		std::unique_ptr<IUIComponent>& in_out_component,
		const UIHierarchyNodeUpdateParam& in_param
		);

#ifdef _DEBUG
public:
	std::string _debug_name;
#endif

private:
	/// don't have a copy of UILayout in the IUIComponent anymore, IUIComponent has UIGeometry though
	/// modification of layout should invalidate the Layout flag
	UILayout _layout;
	/// modification of basecolour should invalidate the Layout flag? needs to rebuild texture hierarchy?
	UIBaseColour _base_colour;

	/// what flag should be changed? effects render
	UITintColour _tint_colour;

	/// parent can be null if we are the root node
	UIData* _parent_or_null;
	/// children array, currently no recusion protection
	std::vector<std::shared_ptr<UIData>> _array_child_data;

	/// array of effects on the node
	std::vector<std::shared_ptr<UIEffectData>> _array_effect_data;

	/// TODO: should the dirty flag have been in the IUIComponent, what if we want to reuse UIData multiple times on screen at once.
	/// to help speedup UI update, have a change tree, if a child has a dirty flag set, also set parent
	UIDataDirty _dirty_flag;

};
