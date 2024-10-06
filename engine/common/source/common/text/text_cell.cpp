#include "common/common_pch.h"
#include "common/text/text_cell.h"

TextCell::TextCell(
	const VectorInt2& in_width_height,
	const VectorInt2& in_bearing,
	const VectorFloat4& in_uv,
	const int in_mask
	)
	: _width_height(in_width_height)
	, _bearing(in_bearing)
	, _uv(in_uv)
	, _mask(in_mask)
{
	// Nop
}

const VectorInt2 TextCell::GetWidthHeight(const float in_ui_scale) const
{
	return VectorInt2(
		static_cast<int>(round(_width_height.GetX() * in_ui_scale)),
		static_cast<int>(round(_width_height.GetY() * in_ui_scale))
		);
}

const VectorInt2 TextCell::GetBearing(const float in_ui_scale) const
{
	return VectorInt2(
		static_cast<int>(round(_bearing.GetX() * in_ui_scale)),
		static_cast<int>(round(_bearing.GetY() * in_ui_scale))
		);
}
