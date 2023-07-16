#include "CommonPCH.h"
#include "Common/Math/VectorFloat2.h"
#include "Common/Math/VectorMath.h"

VectorFloat2::VectorFloat2(const float x, const float y)
	: m_data{ x,y }
{
	return;
}

float& VectorFloat2::operator[](const int index)
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	static float s_dummy = 0;
	return s_dummy;
}

const float VectorFloat2::operator[](const int index)const
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	return 0;
}

const float VectorFloat2::GetX() const
{
	return m_data[Index::X];
}

const float VectorFloat2::GetY() const
{
	return m_data[Index::Y];
}

void VectorFloat2::NormaliseSelf()
{
	const float lengthSquared = ((m_data[X] * m_data[X]) + (m_data[Y] * m_data[Y]));
	if (0.0f != lengthSquared)
	{
		const float length = sqrt(lengthSquared);
		if (0.0f != length)
		{
			const float mul = 1.0f / length;
			m_data[0] *= mul;
			m_data[1] *= mul;
		}
	}

}

const VectorFloat2 VectorFloat2::Cross(const VectorFloat2& subject)
{
	VectorFloat2 result(subject.m_data[Y], -(subject.m_data[X]));
	return result;
}

const bool VectorFloat2::operator==(const VectorFloat2& rhs) const
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
