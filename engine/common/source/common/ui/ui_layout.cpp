#include "common/common_pch.h"
#include "common/ui/ui_layout.h"
#include "common/math/dsc_math.h"

const UILayout UILayout::FactoryFull()
{
	return UILayout(
		UICoord(UICoord::TSource::ParentX, 1.0f, 0.0f),
		UICoord(UICoord::TSource::ParentY, 1.0f, 0.0f)
		);
}

const UILayout UILayout::FactoryParentMiddleQuater()
{
	return UILayout(
		UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
		UICoord(UICoord::TSource::ParentY, 0.5f, 0.0f),
		VectorFloat2(0.5f, 0.5f)
		);
}

UILayout::UILayout(
	const UICoord& in_layout_size_x,
	const UICoord& in_layout_size_y,
	const VectorFloat2& in_layout_space_bias,
	const TAdjustmentType in_adjustment_type_x,
	const TAdjustmentType in_adjustment_type_y,
	const VectorInt4& in_texture_margin, // left, top, right, bottom
	const VectorFloat2& in_adjustment_space_bias
	)
	: _layout_space_bias(in_layout_space_bias)
	, _adjustment_space_bias(in_adjustment_space_bias)
	, _texture_margin(in_texture_margin)
{
	_layout_size[0] = in_layout_size_x;
	_layout_size[1] = in_layout_size_y;
	_adjustment_type[0] = in_adjustment_type_x;
	_adjustment_type[1] = in_adjustment_type_y;
	return;
}

const VectorInt2 UILayout::CalculateLayoutSize(const VectorInt2& in_parent_size, const float in_ui_scale) const
{
	return VectorInt2(
		_layout_size[0].Calculate(in_parent_size, in_ui_scale),
		_layout_size[1].Calculate(in_parent_size, in_ui_scale)
		);
}

const VectorInt2 UILayout::CalculateLayoutOffset(const VectorInt2& in_parent_window_size, const VectorInt2& in_layout_size) const
{
	return VectorInt2(
		static_cast<int>(round((in_parent_window_size[0] - in_layout_size[0]) * _layout_space_bias[0])),
		static_cast<int>(round((in_parent_window_size[1] - in_layout_size[1]) * _layout_space_bias[1]))
		);
}

const VectorInt2 UILayout::CalculateMarginOffset(const float in_ui_scale) const
{
	/// margin [left, top, right, bottom], coords have 0,0 in bottom left
	return VectorInt2(
		static_cast<int>(round(_texture_margin[0] * in_ui_scale)),
		static_cast<int>(round(_texture_margin[3] * in_ui_scale))
		);
}

void UILayout::Finalise(
	VectorInt2& out_layout_size,
	VectorInt2& out_texture_size,
	VectorInt2& out_adjust_offset,
	const VectorInt2& in_layout_size,
	const VectorInt2& in_texture_size
	) const
{
	switch(_adjustment_type[0])
	{
	default:
	case TAdjustmentType::GrowTextureToLayout:
		out_layout_size[0] = in_layout_size[0];
		out_texture_size[0] = std::max(in_texture_size[0], in_layout_size[0]);
		break;
	case TAdjustmentType::ShrinkLayoutToTexture:
		out_layout_size[0] = std::min(in_texture_size[0], in_layout_size[0]);
		out_texture_size[0] = in_texture_size[0];
		break;
	case TAdjustmentType::MatchLayoutToTexture:
		out_layout_size[0] = in_texture_size[0];
		out_texture_size[0] = in_texture_size[0];
		break;
	}

	switch(_adjustment_type[1])
	{
	default:
	case TAdjustmentType::GrowTextureToLayout:
		out_layout_size[1] = in_layout_size[1];
		out_texture_size[1] = std::max(in_texture_size[1], in_layout_size[1]);
		break;
	case TAdjustmentType::ShrinkLayoutToTexture:
		out_layout_size[1] = std::min(in_texture_size[1], in_layout_size[1]);
		out_texture_size[1] = in_texture_size[1];
		break;
	case TAdjustmentType::MatchLayoutToTexture:
		out_layout_size[1] = in_texture_size[1];
		out_texture_size[1] = in_texture_size[1];
		break;
	}

	out_adjust_offset = VectorInt2(
		static_cast<int>(round((out_texture_size[0] - in_texture_size[0]) * _adjustment_space_bias[0])),
		static_cast<int>(round((out_texture_size[1] - in_texture_size[1]) * _adjustment_space_bias[1]))
		);

	return;
}

const VectorInt2 UILayout::AddMargin(
	const VectorInt2& in_size,
	const float in_ui_scale
	) const
{
	return VectorInt2(
		in_size[0] + static_cast<int>(roundf((_texture_margin[0] + _texture_margin[2]) * in_ui_scale)),
		in_size[1] + static_cast<int>(roundf((_texture_margin[1] + _texture_margin[3]) * in_ui_scale))
		);
}

const VectorInt2 UILayout::SubtractMargin(
	const VectorInt2& in_size,
	const float in_ui_scale
	) const
{
	return VectorInt2(
		in_size[0] - static_cast<int>(roundf((_texture_margin[0] + _texture_margin[2]) * in_ui_scale)),
		in_size[1] - static_cast<int>(roundf((_texture_margin[1] + _texture_margin[3]) * in_ui_scale))
		);
}

/// Return true if either axis adjustment modifies the layout size
const bool UILayout::GetAdjustmentModifiesLayout() const
{
	if ((_adjustment_type[0] == TAdjustmentType::GrowTextureToLayout) &&
		(_adjustment_type[1] == TAdjustmentType::GrowTextureToLayout))
	{
		return false;
	}

	return true;
}

/*
void UILayout::SetSliderHorizontal(const float in_value)
{
	_parent_window_attach_ratio[0] = in_value;
	_layout_pivot_ratio[0] = in_value;
	return;
}

void UILayout::SetSliderVertical(const float in_value)
{
	_parent_window_attach_ratio[1] = in_value;
	_layout_pivot_ratio[1] = in_value;
	return;
}

void UILayout::SetScrollHorizontal(const VectorFloat2& in_value)
{
	_size[0] = UICoord(UICoord::TSource::ParentX, in_value[1] - in_value[0]);
	_parent_window_attach_ratio[0] = in_value[0];
	_layout_pivot_ratio[0] = 0.0f;
}

void UILayout::SetScrollVertical(const VectorFloat2& in_value)
{
	_size[1] = UICoord(UICoord::TSource::ParentY, in_value[1] - in_value[0]);
	_parent_window_attach_ratio[1] = 1.0f - in_value[0];
	_layout_pivot_ratio[1] = 1.0f;
}

void UILayout::SetTooltip(const VectorFloat2& in_tooltip_pos)
{
	_parent_window_attach_ratio = in_tooltip_pos;
	_layout_pivot_ratio[0] = 0.5f;
	_layout_pivot_ratio[1] = 0.0f;
}

void UILayout::SetComboBoxDropdown(const VectorFloat4& in_combo_box_screen_pos)
{
	const float width = (in_combo_box_screen_pos[2] - in_combo_box_screen_pos[0]) * 0.5f;
	const float mid_x = (in_combo_box_screen_pos[2] + in_combo_box_screen_pos[0]) * 0.5f;
	_size[0] = UICoord(UICoord::TSource::ParentX, width);
	_parent_window_attach_ratio[0] = DscMath::ConvertNegativeOneOneToZeroOne(mid_x);
	_parent_window_attach_ratio[1] = DscMath::ConvertNegativeOneOneToZeroOne(in_combo_box_screen_pos[1]);
	_layout_pivot_ratio[0] = 0.5f;
	_layout_pivot_ratio[1] = 1.0f;
}
*/

const bool UILayout::SetLayoutSpaceBias(const VectorFloat2& in_layout_space_bias)
{
	bool dirty = false;
	if (_layout_space_bias != in_layout_space_bias)
	{
		dirty = true;
		_layout_space_bias = in_layout_space_bias;
	}
	return dirty;
}

const bool UILayout::operator==(const UILayout& in_rhs) const
{
	if (_layout_size[0] != in_rhs._layout_size[0])
	{
		return false;
	}
	if (_layout_size[1] != in_rhs._layout_size[1])
	{
		return false;
	}

	if (_layout_space_bias != in_rhs._layout_space_bias)
	{
		return false;
	}

	if (_adjustment_space_bias != in_rhs._adjustment_space_bias)
	{
		return false;
	}

	if (_adjustment_type[0] != in_rhs._adjustment_type[0])
	{
		return false;
	}

	if (_adjustment_type[1] != in_rhs._adjustment_type[1])
	{
		return false;
	}

	if (_texture_margin != in_rhs._texture_margin)
	{
		return false;
	}

	return true;
}

const bool UILayout::operator!=(const UILayout& in_rhs) const
{
	return !operator==(in_rhs);
}
