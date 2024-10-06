#include "common/common_pch.h"

#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_float3.h"

// Http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
const MatrixFloat33 MatrixFloat33::FactoryQuaternionFloat(const QuaternionFloat&in_rotation)
{
	const float w = in_rotation.GetW();
	const float x = in_rotation.GetX();
	const float y = in_rotation.GetY();
	const float z = in_rotation.GetZ();
	const float xx = x* x;
	const float xy = x* y;
	const float xz = x* z;
	const float xw = x* w;
	const float yy = y* y;
	const float yz = y* z;
	const float yw = y* w;
	const float zz = z* z;
	const float zw = z* w;
	const float m00 = 1.0f - 2.0f* (yy + zz);
	const float m01 = 2.0f* (xy - zw);
	const float m02 = 2.0f* (xz + yw);
	const float m10 = 2.0f* (xy + zw);
	const float m11 = 1.0f - 2.0f* (xx + zz);
	const float m12 = 2.0f* (yz - xw);
	const float m20 = 2.0f* (xz - yw);
	const float m21 = 2.0f* (yz + xw);
	const float m22 = 1.0f - 2.0f* (xx + yy);
	return MatrixFloat33(
		m00,
		m10,
		m20,
		m01,
		m11,
		m21,
		m02,
		m12,
		m22
		);
}

MatrixFloat33::MatrixFloat33(
	const float in_a00,
	const float in_a10,
	const float in_a20,
	const float in_a01,
	const float in_a11,
	const float in_a21,
	const float in_a02,
	const float in_a12,
	const float in_a22
	) : _data{ in_a00, in_a10, in_a20, in_a01, in_a11, in_a21, in_a02, in_a12, in_a22}

{
	return;
}
float&MatrixFloat33::operator[](const int in_index)
{
	if ((0 <= in_index) && (in_index < Index::Count))
	{
		return _data[in_index];
	}
	static float s_dummy = 0;
	return s_dummy;
}

const float MatrixFloat33::operator[](const int in_index) const
{
	if ((0 <= in_index) && (in_index < Index::Count))
	{
		return _data[in_index];
	}
	return 0;
}

const bool MatrixFloat33::operator==(const MatrixFloat33&in_rhs) const
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

const VectorFloat3 operator* (const MatrixFloat33& in_lhs, const VectorFloat3&in_rhs)
{
	const float x = in_rhs.GetX();
	const float y = in_rhs.GetY();
	const float z = in_rhs.GetZ();
#if 0
	return VectorFloat3(
		(x * in_lhs._data[MatrixFloat33::M00]) + (y * in_lhs._data[MatrixFloat33::M01]) + (z * in_lhs._data[MatrixFloat33::M02]),
		(x * in_lhs._data[MatrixFloat33::M10]) + (y * in_lhs._data[MatrixFloat33::M11]) + (z * in_lhs._data[MatrixFloat33::M12]),
		(x * in_lhs._data[MatrixFloat33::M20]) + (y * in_lhs._data[MatrixFloat33::M21]) + (z * in_lhs._data[MatrixFloat33::M22])
		);
#else
	return VectorFloat3(
		(x * in_lhs._data[MatrixFloat33::M00]) + (y * in_lhs._data[MatrixFloat33::M10]) + (z * in_lhs._data[MatrixFloat33::M20]),
		(x * in_lhs._data[MatrixFloat33::M01]) + (y * in_lhs._data[MatrixFloat33::M11]) + (z * in_lhs._data[MatrixFloat33::M21]),
		(x * in_lhs._data[MatrixFloat33::M02]) + (y * in_lhs._data[MatrixFloat33::M12]) + (z * in_lhs._data[MatrixFloat33::M22])
		);
#endif
}

