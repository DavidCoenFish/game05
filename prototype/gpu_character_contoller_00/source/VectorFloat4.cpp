#include "pch.h"
#include "VectorFloat4.h"

VectorFloat4::VectorFloat4(const float x, const float y, const float z, const float w)
	: m_data{ x,y,z,w }
{
	return;
}

float& VectorFloat4::operator[](const int index)
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	static float s_dummy = 0;
	return s_dummy;
}

const float VectorFloat4::operator[](const int index)const
{
	if ((0 <= index) && (index < Index::Count))
	{
		return m_data[index];
	}
	return 0;
}

const bool VectorFloat4::operator==(const VectorFloat4& rhs) const
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

