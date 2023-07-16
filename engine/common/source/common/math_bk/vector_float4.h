#pragma once

class VectorFloat4
{
public:
	explicit VectorFloat4(const float in_x = 0, const float in_y = 0, const float in_z = 0, const float in_w = 0);

	float& operator[](const int in_index);
	const float operator[](const int in_index)const;

	const bool operator==(const VectorFloat4& in_rhs) const;

private:
	enum Index
	{
		X = 0,
		Y,
		Z,
		W,
		Count
	};
	float _data[Index::Count];
};


