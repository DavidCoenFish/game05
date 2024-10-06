#include "common/common_pch.h"

#include "common/math/quaternion_float.h"
#include "common/math/vector_float3.h"

const QuaternionFloat QuaternionFloat::FactoryIdentity()
{
	return QuaternionFloat(
		0.0f,
		0.0f,
		0.0f,
		1.0f
		);
}

const QuaternionFloat QuaternionFloat::FactoryAxisAngle(
	const VectorFloat3& in_axis,
	const float in_angle_radian
	)
{
	const float sin_angle = sin(in_angle_radian* 0.5f);
	const float cos_angle = cos(in_angle_radian* 0.5f);
	return QuaternionFloat(
		in_axis[0]* sin_angle,
		in_axis[1]* sin_angle,
		in_axis[2]* sin_angle,
		cos_angle
		);
}

QuaternionFloat::QuaternionFloat(
	const float in_x,
	const float in_y,
	const float in_z,
	const float in_w
	) : _data{ in_x, in_y, in_z, in_w}

{
	return;
}
float&QuaternionFloat::operator[](const int in_index)
{
	if ((0 <= in_index) && (in_index < Index::Count))
	{
		return _data[in_index];
	}
	static float s_dummy = 0;
	return s_dummy;
}

const float QuaternionFloat::operator[](const int in_index) const
{
	if ((0 <= in_index) && (in_index < Index::Count))
	{
		return _data[in_index];
	}
	return 0;
}

const bool QuaternionFloat::operator==(const QuaternionFloat& in_rhs) const
{
	for (int index = 0; index < Index::Count;++ index)
	{
		if (_data[index] != in_rhs._data[index])
		{
			return false;
		}
	}
	return true;
}

const float QuaternionFloat::GetX() const
{
	return _data[Index::X];
}

const float QuaternionFloat::GetY() const
{
	return _data[Index::Y];
}

const float QuaternionFloat::GetZ() const
{
	return _data[Index::Z];
}

const float QuaternionFloat::GetW() const
{
	return _data[Index::W];
}

QuaternionFloat& operator*= (QuaternionFloat& in_lhs, const QuaternionFloat& in_rhs)
{
	in_lhs = in_lhs * in_rhs;
	return in_lhs;
}

const QuaternionFloat operator* (const QuaternionFloat& in_lhs, const QuaternionFloat& in_rhs)
{
	return QuaternionFloat(
		(in_lhs._data[3] * in_rhs._data[0]) + (in_lhs._data[0] * in_rhs._data[3]) + (in_lhs._data[1] * in_rhs._data[2]) - (in_lhs._data[2] * in_rhs.\
			_data[1]),
		(in_lhs._data[3] * in_rhs._data[1]) - (in_lhs._data[0] * in_rhs._data[2]) + (in_lhs._data[1] * in_rhs._data[3]) + (in_lhs._data[2] * in_rhs.\
			_data[0]),
		(in_lhs._data[3] * in_rhs._data[2]) + (in_lhs._data[0] * in_rhs._data[1]) - (in_lhs._data[1] * in_rhs._data[0]) + (in_lhs._data[2] * in_rhs.\
			_data[3]),
		(in_lhs._data[3] * in_rhs._data[3]) - (in_lhs._data[0] * in_rhs._data[0]) - (in_lhs._data[1] * in_rhs._data[1]) - (in_lhs._data[2] * in_rhs.\
			_data[2])
	);
}


