#include "common/common_pch.h"

#include "common/math/vector_int2.h"

const VectorInt2 VectorInt2::s_zero(0, 0);

VectorInt2::VectorInt2(
    const int in_x,
    const int in_y
    ) : _data{ in_x, in_y}

{
    return;
}
int& VectorInt2::operator[](const int in_index)
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    static int s_dummy = 0;
    return s_dummy;
}

const int VectorInt2::operator[](const int in_index) const
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    return 0;
}

const int VectorInt2::GetX() const
{
    return _data[Index::X];
}

const int VectorInt2::GetY() const
{
    return _data[Index::Y];
}

const bool VectorInt2::operator==(const VectorInt2& in_rhs) const
{
    for (int index = 0; index < Index::Count;++ index)
    {
        if (_data[index] != in_rhs._data[index])
        {
            return false;
        }
    }
    return true;
}

const bool VectorInt2::operator!=(const VectorInt2& in_rhs) const
{
    return !(operator==(in_rhs));
}

const VectorInt2 VectorInt2::Max(const VectorInt2& in_lhs, const VectorInt2& in_rhs)
{
    return VectorInt2(
        std::max(in_lhs._data[0], in_rhs._data[0]),
        std::max(in_lhs._data[1], in_rhs._data[1])
        );
}

const VectorInt2 operator+(const VectorInt2& in_lhs, const VectorInt2& in_rhs)
{
    return VectorInt2(
        in_lhs.GetX() + in_rhs.GetX(),
        in_lhs.GetY() + in_rhs.GetY()
        );
}
