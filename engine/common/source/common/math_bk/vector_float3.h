#pragma once

class VectorFloat3
{
public:
	explicit VectorFloat3(const float x = 0, const float y = 0, const float z = 0);

	float& operator[](const int index);
	const float operator[](const int index)const;

	const float GetX() const;
	const float GetY() const;
	const float GetZ() const;

	void NormaliseSelf();

	const bool operator==(const VectorFloat3& rhs) const;
	static const VectorFloat3 Cross(const VectorFloat3& lhs, const VectorFloat3& rhs);

private:
	enum Index
	{
		X = 0,
		Y,
		Z,
		Count
	};
	float m_data[Index::Count];

};


