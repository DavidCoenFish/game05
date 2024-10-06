#include "common/common_pch.h"
#include "common/ui/ui_data/ui_data_canvas.h"

#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_screen_space.h"

UIDataCanvas::UIDataCanvas(
#ifdef _DEBUG
	const std::string& in_debug_name,
#endif
	const UILayout& in_layout,
	const UIBaseColour& in_base_colour,
	const UITintColour& in_tint_colour,
	const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data
	)
	: UIData(
#ifdef _DEBUG
		in_debug_name,
#endif
		in_layout,
		in_base_colour,
		in_tint_colour,
		in_array_effect_data
		)
{
	// Nop
}

const bool UIDataCanvas::ApplyComponent(
	std::unique_ptr<IUIComponent>& in_out_component,
	const UIHierarchyNodeUpdateParam& //in_param
	)
{
	bool dirty = false;
	// if in_out_component is not a UIComponentCanvas, remake it as one
	UIComponentCanvas* component = dynamic_cast<UIComponentCanvas*>(in_out_component.get());
	if (nullptr == component)
	{
		auto new_component = std::make_unique<UIComponentCanvas>();
		//component = new_component.get();
		in_out_component = std::move(new_component);
		dirty = true;
	}

	return dirty;
}
