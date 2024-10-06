#pragma once

#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class TextCell
{
public:
	TextCell(
		const VectorInt2& in_width_height,
		const VectorInt2& in_bearing,
		const VectorFloat4& in_uv,
		const int in_mask
		);

	const VectorInt2& GetWidthHeightRef() const { return _width_height; }
	const VectorInt2 GetWidthHeight(const float in_ui_scale) const;
	const VectorInt2& GetBearingRef() const { return _bearing; }
	const VectorInt2 GetBearing(const float in_ui_scale) const;
	const VectorFloat4& GetUVRef() const { return _uv; }
	int GetMask() const { return _mask; }

private:
	VectorInt2 _width_height;
	VectorInt2 _bearing;
	VectorFloat4 _uv;
	int _mask;

};
