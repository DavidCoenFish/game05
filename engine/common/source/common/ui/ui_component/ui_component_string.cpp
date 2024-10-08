#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_string.h"

#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/draw_system.h"
#include "common/math/dsc_math.h"
#include "common/text/text_block.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_hierarchy_node_child.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"


UIComponentString::UIComponentString(
	std::unique_ptr<TextBlock>& in_text_block
	)
	: IUIComponent()
	, _text_block(std::move(in_text_block))
{
	return;
}

UIComponentString::~UIComponentString()
{
	return;
}

const bool UIComponentString::SetText(
	const std::string& in_string_utf8,
	const TextLocale* const in_locale_token
	)
{
	bool dirty = false;
	if (true == _text_block->SetText(in_string_utf8, in_locale_token))
	{
		dirty = true;
	}
	return dirty;
}

const bool UIComponentString::Set(
	TextFont& in_font, 
	const int in_font_size,
	const float in_new_line_gap_ratio,
	const bool in_width_limit_enabled,
	const TextEnum::HorizontalLineAlignment in_horizontal, 
	const TextEnum::VerticalBlockAlignment in_vertical,
	const VectorFloat4& in_text_colour
	)
{
	bool dirty = false;

	if (true == _text_block->SetFont(in_font))
	{
		dirty = true;
	}
	if (true == _text_block->SetFontSize(in_font_size))
	{
		dirty = true;
	}
	if (true == _text_block->SetNewLineGapRatio(in_new_line_gap_ratio))
	{
		dirty = true;
	}
	if (true == _text_block->SetWidthLimitEnabled(in_width_limit_enabled))
	{
		dirty = true;
	}
	if (true == _text_block->SetHorizontalLineAlignment(in_horizontal))
	{
		dirty = true;
	}
	if (true == _text_block->SetVerticalBlockAlignment(in_vertical))
	{
		dirty = true;
	}
	if (true == _text_block->SetColour(in_text_colour))
	{
		dirty = true;
	}

	return dirty;
}

const VectorInt2 UIComponentString::GetDesiredSize(
	UIHierarchyNodeChild& in_component_owner,
	const UIHierarchyNodeUpdateParam& in_layout_param,
	const VectorInt2& in_pre_shrink_layout_size //in_parent_window
	)
{
	// todo: move margin into text block, for now append margin to bounds.
	_text_block->SetWidthLimit(
		_text_block->GetWidthLimitEnabled(),
		in_pre_shrink_layout_size[0]
		);
	_text_block->SetUIScale(in_layout_param._ui_scale);

	const VectorInt2 bounds = _text_block->GetTextBounds();
	const VectorInt2 result = in_component_owner.GetLayout().AddMargin(bounds, in_layout_param._ui_scale);
	return result;
}

void UIComponentString::UpdateResources(
	UIHierarchyNodeChild& in_component_owner,
	const UIHierarchyNodeUpdateParam& in_param,
	const UIScreenSpace& in_parent_screen_space,
	const VectorInt2& in_parent_texture_size,
	const VectorInt2& in_parent_window,
	const VectorInt2& in_parent_offset,
	const VectorInt4& in_texture_margin,
	const VectorInt2& in_parent_adjust_offset
	)
{
	TSuper::UpdateResources(
		in_component_owner,
		in_param,
		in_parent_screen_space,
		in_parent_texture_size,
		in_parent_window,
		in_parent_offset,
		in_texture_margin,
		in_parent_adjust_offset
		);

	if (true == _text_block->SetTextContainerSize(
		in_component_owner.GetNode().GetTextureSize(in_param._draw_system)//,
		//in_parent_adjust_offset
		))
	{
		in_component_owner.SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
	}

	return;
}

void UIComponentString::PreDraw(
	const UIManagerDrawParam& in_draw_param,
	UIHierarchyNode& in_node
	) 
{
	auto& texture = in_node.GetBaseUITexture();

	// we are only in component predraw if the external node child state has allowed us to draw
	//if (true == texture.CalculateNeedsToDraw())
	if (false == texture.SetRenderTarget(
		in_draw_param._draw_system,
		in_draw_param._frame
		))
	{
		return;
	}

	in_draw_param._text_manager->DrawText(
		in_draw_param._draw_system,
		in_draw_param._frame,
		_text_block.get()
		);

	// don't mark the texture as drawn to allow our owning node to still consider drawing to it, in particular for the effect stack draw
	//texture.SetHasDrawn(true);

	return;
}
