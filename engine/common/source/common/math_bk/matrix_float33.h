#pragma once

class QuaternionFloat;
class VectorFloat3;
class MatrixFloat33;

class MatrixFloat33
{
public:
	static const MatrixFloat33 FactoryQuaternionFloat(const QuaternionFloat& rotation);

	explicit MatrixFloat33(
		const float a00 = 0, const float a10 = 0, const float a20 = 0,
		const float a01 = 0, const float a11 = 0, const float a21 = 0,
		const float a02 = 0, const float a12 = 0, const float a22 = 0
		);

	float& operator[](const int index);
	const float operator[](const int index)const;

	const bool operator==(const MatrixFloat33& rhs) const;

	const VectorFloat3 operator*(const VectorFloat3& rhs) const;
private:
	enum Index
	{
		M00 = 0,
		M10,
		M20,
		M01,
		M11,
		M21,
		M02,
		M12,
		M22,
		Count
	};
	float m_data[Index::Count];
};


