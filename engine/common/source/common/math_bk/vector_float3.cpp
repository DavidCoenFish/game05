#include "CommonPCH.h"
#include "Common/Math/VectorFloat3.h"
#include "Common/Math/VectorMath.h"

VectorFloat3::VectorFloat3(const float x, const float y, const float z)
	: m_data{ x,y,z }
{
	return;
}

float& VectorFloat3::operator[](const int index)
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	static float s_dummy = 0;
	return s_dummy;
}

const float VectorFloat3::operator[](const int index)const
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	return 0;
}

const float VectorFloat3::GetX() const
{
	return m_data[Index::X];
}

const float VectorFloat3::GetY() const
{
	return m_data[Index::Y];
}

const float VectorFloat3::GetZ() const
{
	return m_data[Index::Z];
}

void VectorFloat3::NormaliseSelf()
{
	const float lengthSquared = ((m_data[X] * m_data[X]) + (m_data[Y] * m_data[Y]) + (m_data[Z] * m_data[Z]));
	if (0.0f != lengthSquared)
	{
		const float length = sqrt(lengthSquared);
		if (0.0f != length)
		{
			const float mul = 1.0f / length;
			m_data[0] *= mul;
			m_data[1] *= mul;
			m_data[2] *= mul;
		}
	}

}

const VectorFloat3 VectorFloat3::Cross(const VectorFloat3& lhs, const VectorFloat3& rhs)
{
	VectorFloat3 result;
	VectorMath::CrossProduct(result.m_data, lhs.m_data, rhs.m_data);
	return result;
}

const bool VectorFloat3::operator==(const VectorFloat3& rhs) const
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
