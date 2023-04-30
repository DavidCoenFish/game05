#pragma once

class VectorFloat4
{
public:
	explicit VectorFloat4(const float x = 0, const float y = 0, const float z = 0, const float w = 0);

	float& operator[](const int index);
	const float operator[](const int index)const;

	const bool operator==(const VectorFloat4& rhs) const;

private:
	enum Index
	{
		X = 0,
		Y,
		Z,
		W,
		Count
	};
	float m_data[Index::Count];
};


