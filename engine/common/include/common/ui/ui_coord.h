#pragma once

#include "common/math/vector_float2.h"
#include "common/math/vector_int2.h"

class VectorFloat2;
class VectorInt2;

class UICoord
{
public:
	/// Both Parent and Content in the same enum as a UICoord is only one axis, so will not be at risk of mixing content and parent
	enum class TSource
	{
		None,
		ParentX,
		ParentY,
		ParentMin,
		ParentMax,
		// make a 4:3 box in the parent and use the x or y
		ParentRatio_4_3_X,
		ParentRatio_4_3_Y
	};

	explicit UICoord(
		const TSource in_parent_source = TSource::None,
		const float in_ratio = 0.0f,
		const float in_em_offset = 0.0f
		);
	~UICoord();

	const float Calculate(
		const VectorFloat2& in_parent_size,
		const float in_ui_scale
		) const;

	/// certain calculations, like render target size, use int
	const int Calculate(
		const VectorInt2& in_size,
		const float in_ui_scale
		) const;

	const bool operator==(const UICoord& in_rhs) const;
	const bool operator!=(const UICoord& in_rhs) const;

private:
	/// What method should be use to calculate 
	TSource _source;
	/// Ratio applied to value specified by _source
	float _ratio;
	/// pixel offset, will be multiplied by UIScale
	float _em_offset;

};
