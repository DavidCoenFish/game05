#pragma once

class VectorFloat3;
class VectorFloat4;

namespace DscMath
{
	const bool AlmostEqual(const float in_lhs, const float in_rhs, const float in_epsilon = std::numeric_limits<float>::min());

	const float ApproximateDistance(const float in_dx, const float in_dy);
	const float Length(const VectorFloat3& in_value);
	const VectorFloat3 Normal(const VectorFloat3& in_value, const float in_length);

	const int ScaleInt(const int in_value, const float in_scale);
	//constexpr int ScaleInt(constexpr int in_value, constexpr float in_scale);

	const float ConvertZeroOneToNegativeOneOne(const float in_value);
	const VectorFloat4 ConvertZeroOneToNegativeOneOne(const VectorFloat4& in_value);

	const float ConvertNegativeOneOneToZeroOne(const float in_value);
	const VectorFloat4 ConvertNegativeOneOneToZeroOne(const VectorFloat4& in_value);

	/// 0xff000000 a:1.0f
	/// 0x000000ff r:1.0f
	const int ConvertColourToInt(const VectorFloat4& in_value);
	/// map [0.0f ... 1.0f] -> [0 ... 255]
	const int ConvertFloatToByte(const float in_value);

	const int32_t IntCeilingDiv(const int32_t in_x, const int32_t in_y);

}
