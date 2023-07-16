#pragma once

class VectorFloat2
{
public:
	explicit VectorFloat2(const float x = 0, const float y = 0);

	float& operator[](const int index);
	const float operator[](const int index)const;

	const float GetX() const;
	const float GetY() const;

	void NormaliseSelf();

	const bool operator==(const VectorFloat2& rhs) const;
	static const VectorFloat2 Cross(const VectorFloat2& subject);

private:
	enum Index
	{
		X = 0,
		Y,
		Count
	};
	float m_data[Index::Count];

};


