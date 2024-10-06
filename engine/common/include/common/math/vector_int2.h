#pragma once

class VectorInt2
{
public:
	static const VectorInt2 s_zero;

	explicit VectorInt2(
		const int in_x = 0,
		const int in_y = 0
		);
	int& operator[](const int in_index);
	const int operator[](const int in_index) const;
	const int GetX() const;
	const int GetY() const;
	const bool operator==(const VectorInt2& in_rhs) const;
	const bool operator!=(const VectorInt2& in_rhs) const;

	static const VectorInt2 Max(const VectorInt2& in_lhs, const VectorInt2& in_rhs);

private:
	enum Index
	{
		X = 0,
		Y,
		Count
	};

	int _data[Index::Count];

};

const VectorInt2 operator+(const VectorInt2& in_lhs, const VectorInt2& in_rhs);

