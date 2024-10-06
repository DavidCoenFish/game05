#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/text/text_enum.h"

class TextBlock;
class TextFont;
class TextLocale;

class UIComponentString : public IUIComponent
{
	typedef IUIComponent TSuper;
public:
	UIComponentString(
		std::unique_ptr<TextBlock>& in_text_block
		);
	~UIComponentString();

	const bool SetText(
		const std::string& in_string_utf8,
		const TextLocale* const in_locale_token
		);

	// return true if modified, else false
	const bool Set(
		TextFont& in_font, 
		const int in_font_size,
		const float in_new_line_gap_ratio,
		const bool in_width_limit_enabled,
		const TextEnum::HorizontalLineAlignment in_horizontal, 
		const TextEnum::VerticalBlockAlignment in_vertical,
		const VectorFloat4& in_text_colour
		);

private:
	virtual const VectorInt2 GetDesiredSize(
		UIHierarchyNodeChild& in_component_owner,
		const UIHierarchyNodeUpdateParam& in_layout_param,
		const VectorInt2& in_pre_shrink_layout_size //in_parent_window
		) override;

	virtual void UpdateResources(
		UIHierarchyNodeChild& in_component_owner,
		const UIHierarchyNodeUpdateParam& in_param,
		const UIScreenSpace& in_parent_screen_space,
		const VectorInt2& in_parent_texture_size,
		const VectorInt2& in_parent_window,
		const VectorInt2& in_parent_offset,
		const VectorInt4& in_texture_margin,
		const VectorInt2& in_parent_adjust_offset
		) override;

	virtual void PreDraw(
		const UIManagerDrawParam& in_draw_param,
		UIHierarchyNode& in_node
		) override;

private:
	//bool _use_parent_size_for_width_limit;
	std::unique_ptr<TextBlock> _text_block;

};
