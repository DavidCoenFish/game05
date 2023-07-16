#include "CommonPCH.h"
#include "Common/Math/MatrixFloat33.h"
#include "Common/Math/QuaternionFloat.h"
#include "Common/Math/VectorFloat3.h"

//http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
const MatrixFloat33 MatrixFloat33::FactoryQuaternionFloat(const QuaternionFloat& rotation)
{
	const float w = rotation.GetW();
	const float x = rotation.GetX();
	const float y = rotation.GetY();
	const float z = rotation.GetZ();

	const float xx      = x * x;
	const float xy      = x * y;
	const float xz      = x * z;
	const float xw      = x * w;

	const float yy      = y * y;
	const float yz      = y * z;
	const float yw      = y * w;

	const float zz      = z * z;
	const float zw      = z * w;

	const float m00  = 1.0f - 2.0f * ( yy + zz );
	const float m01  =     2.0f * ( xy - zw );
	const float m02 =     2.0f * ( xz + yw );

	const float m10  =     2.0f * ( xy + zw );
	const float m11  = 1.0f - 2.0f * ( xx + zz );
	const float m12  =     2.0f * ( yz - xw );

	const float m20  =     2.0f * ( xz - yw );
	const float m21  =     2.0f * ( yz + xw );
	const float m22 = 1.0f - 2.0f * ( xx + yy );

	return MatrixFloat33(
		m00, m10, m20, 
		m01, m11, m21, 
		m02, m12, m22
		);
}


MatrixFloat33::MatrixFloat33(
	const float a00, const float a10, const float a20,
	const float a01, const float a11, const float a21,
	const float a02, const float a12, const float a22
	)
: m_data{ 
	a00, a10, a20,
	a01, a11, a21,
	a02, a12, a22
	}
{
	return;
}

float& MatrixFloat33::operator[](const int index)
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	static float s_dummy = 0;
	return s_dummy;
}

const float MatrixFloat33::operator[](const int index)const
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	return 0;
}

const bool MatrixFloat33::operator==(const MatrixFloat33& rhs) const
{
	for (int index = 0; index < Index::Count; ++index)
	{
		if (m_data[index] != rhs.m_data[index])
		{
			return false;
		}
	}
	return true;
}

const VectorFloat3 MatrixFloat33::operator*(const VectorFloat3& rhs) const
{
	const float x = rhs.GetX();
	const float y = rhs.GetY();
	const float z = rhs.GetZ();
	return VectorFloat3(
		(x * m_data[M00]) + (y * m_data[M01]) + (z * m_data[M02]),
		(x * m_data[M10]) + (y * m_data[M11]) + (z * m_data[M12]),
		(x * m_data[M20]) + (y * m_data[M21]) + (z * m_data[M22])
		);
}
