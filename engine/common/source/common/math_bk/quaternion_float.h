#pragma once

class VectorFloat3;

class QuaternionFloat
{
public:
	static const QuaternionFloat FactoryIdentity();
	static const QuaternionFloat FactoryAxisAngle(
		const VectorFloat3& axis,
		const float angleRadian
		);

	explicit QuaternionFloat(const float x = 0, const float y = 0, const float z = 0, const float w = 0);

	float& operator[](const int index);
	const float operator[](const int index)const;

	const bool operator==(const QuaternionFloat& rhs) const;
	const QuaternionFloat operator*(const QuaternionFloat& rhs) const;

	const float GetX() const;
	const float GetY() const;
	const float GetZ() const;
	const float GetW() const;

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


