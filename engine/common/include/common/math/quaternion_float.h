#pragma once
class VectorFloat3;

class QuaternionFloat
{
public:
	static const QuaternionFloat FactoryIdentity();
	static const QuaternionFloat FactoryAxisAngle(
		const VectorFloat3& in_axis,
		const float in_angle_radian
		);
	explicit QuaternionFloat(
		const float in_x = 0,
		const float in_y = 0,
		const float in_z = 0,
		const float in_w = 0
		);
	float& operator[](const int in_index);
	const float operator[](const int in_index) const;
	const bool operator==(const QuaternionFloat& in_rhs) const;
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
		Count};

private:
	float _data[Index::Count];

	friend QuaternionFloat& operator*= (QuaternionFloat& in_lhs, const QuaternionFloat& in_rhs);
	friend const QuaternionFloat operator* (const QuaternionFloat& in_lhs, const QuaternionFloat& in_rhs);

};

QuaternionFloat& operator*= (QuaternionFloat& in_lhs, const QuaternionFloat& in_rhs);
const QuaternionFloat operator* (const QuaternionFloat& in_lhs, const QuaternionFloat& in_rhs);
