#pragma once
class VectorFloat3
{
public:
	explicit VectorFloat3(
		const float in_x = 0,
		const float in_y = 0,
		const float in_z = 0
		);

	const float GetX() const;
	const float GetY() const;
	const float GetZ() const;
	void NormaliseSelf();

	float& operator[](const int in_index);
	const float operator[](const int in_index) const;
	const bool operator==(const VectorFloat3& in_rhs) const;

	friend const VectorFloat3 Cross(
		const VectorFloat3& in_lhs,
		const VectorFloat3& in_rhs
		);
	friend const float Dot(
		const VectorFloat3& in_lhs,
		const VectorFloat3& in_rhs
		);
	friend VectorFloat3& operator+= (VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
	friend VectorFloat3& operator-= (VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
	friend const VectorFloat3 operator+ (const VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
	friend const VectorFloat3 operator- (const VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
	friend const VectorFloat3 operator* (const VectorFloat3& in_lhs, const float in_rhs);
	friend const VectorFloat3 operator* (const float in_lhs, const VectorFloat3& in_rhs);

private:
	enum Index
	{
		X = 0,
		Y,
		Z,
		Count};

private:
	float _data[Index::Count];

};

const VectorFloat3 Cross(
	const VectorFloat3& in_lhs,
	const VectorFloat3& in_rhs
	);
const float Dot(
	const VectorFloat3& in_lhs,
	const VectorFloat3& in_rhs
	);
VectorFloat3& operator+= (VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
VectorFloat3& operator-= (VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
const VectorFloat3 operator+ (const VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
const VectorFloat3 operator- (const VectorFloat3& in_lhs, const VectorFloat3& in_rhs);
const VectorFloat3 operator* (const VectorFloat3& in_lhs, const float in_rhs);
const VectorFloat3 operator* (const float in_lhs, const VectorFloat3& in_rhs);
