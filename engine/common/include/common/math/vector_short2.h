#pragma once

class VectorShort2
{
public:
	explicit VectorShort2(
		const short in_x = 0,
		const short in_y = 0
		);

	const short GetX() const;
	const short GetY() const;

	short& operator[](const int in_index);
	const short operator[](const int in_index) const;

	const bool operator==(const VectorShort2& in_rhs) const;
	const bool operator!=(const VectorShort2& in_rhs) const;

private:
	enum Index
	{
		X = 0,
		Y,
		Count
	};

private:
	short _data[Index::Count];

};
