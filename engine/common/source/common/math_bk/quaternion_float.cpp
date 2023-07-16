#include "CommonPCH.h"
#include "Common/Math/QuaternionFloat.h"
#include "Common/Math/VectorFloat3.h"

const QuaternionFloat QuaternionFloat::FactoryIdentity()
{
	return QuaternionFloat(0.0f, 0.0f, 0.0f, 1.0f);
}

const QuaternionFloat QuaternionFloat::FactoryAxisAngle(
	const VectorFloat3& axis,
	const float angleRadian
	)
{
	const float sinAngle = sin(angleRadian * 0.5f);
	const float cosAngle = cos(angleRadian * 0.5f);
	return QuaternionFloat(
		axis[0] * sinAngle,
		axis[1] * sinAngle,
		axis[2] * sinAngle,
		cosAngle
		);
}

QuaternionFloat::QuaternionFloat(const float x, const float y, const float z, const float w)
	: m_data{ x,y,z,w }
{
	return;
}

float& QuaternionFloat::operator[](const int index)
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	static float s_dummy = 0;
	return s_dummy;
}

const float QuaternionFloat::operator[](const int index)const
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	return 0;
}

const bool QuaternionFloat::operator==(const QuaternionFloat& rhs) const
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

const QuaternionFloat QuaternionFloat::operator*(const QuaternionFloat& rhs) const
{
	return QuaternionFloat(
		(m_data[3] * rhs.m_data[0]) + (m_data[0] * rhs.m_data[3]) + (m_data[1] * rhs.m_data[2]) - (m_data[2] * rhs.m_data[1]),
		(m_data[3] * rhs.m_data[1]) - (m_data[0] * rhs.m_data[2]) + (m_data[1] * rhs.m_data[3]) + (m_data[2] * rhs.m_data[0]),
		(m_data[3] * rhs.m_data[2]) + (m_data[0] * rhs.m_data[1]) - (m_data[1] * rhs.m_data[0]) + (m_data[2] * rhs.m_data[3]),
		(m_data[3] * rhs.m_data[3]) - (m_data[0] * rhs.m_data[0]) - (m_data[1] * rhs.m_data[1]) - (m_data[2] * rhs.m_data[2])
	);
}

const float QuaternionFloat::GetX() const
{
	return m_data[Index::X];
}
const float QuaternionFloat::GetY() const
{
	return m_data[Index::Y];
}
const float QuaternionFloat::GetZ() const
{
	return m_data[Index::Z];
}
const float QuaternionFloat::GetW() const
{
	return m_data[Index::W];
}
