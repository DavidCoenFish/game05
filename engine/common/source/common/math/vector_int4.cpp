#include "common/common_pch.h"

#include "common/math/vector_int4.h"

const VectorInt4 VectorInt4::s_zero(0, 0, 0, 0);

VectorInt4::VectorInt4(
    const int in_x,
    const int in_y,
    const int in_z,
    const int in_w
    ) 
    : _data{in_x, in_y, in_z, in_w}
{
    return;
}
int& VectorInt4::operator[](const int in_index)
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    static int s_dummy = 0;
    return s_dummy;
}

const int VectorInt4::operator[](const int in_index) const
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    return 0;
}

const int VectorInt4::GetX() const
{
    return _data[Index::X];
}

const int VectorInt4::GetY() const
{
    return _data[Index::Y];
}

const int VectorInt4::GetZ() const
{
    return _data[Index::Z];
}

const int VectorInt4::GetW() const
{
    return _data[Index::W];
}

const bool VectorInt4::operator==(const VectorInt4& in_rhs) const
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

