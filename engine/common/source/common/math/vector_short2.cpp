#include "common/common_pch.h"

#include "common/math/vector_short2.h"

VectorShort2::VectorShort2(
    const short in_x,
    const short in_y
    ) 
    : _data{ in_x, in_y}
{
    return;
}

const short VectorShort2::GetX() const
{
    return _data[Index::X];
}

const short VectorShort2::GetY() const
{
    return _data[Index::Y];
}

short& VectorShort2::operator[](const int in_index)
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    static short s_dummy = 0;
    return s_dummy;
}

const short VectorShort2::operator[](const int in_index) const
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    return 0;
}

const bool VectorShort2::operator==(const VectorShort2& in_rhs) const
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

const bool VectorShort2::operator!=(const VectorShort2& in_rhs) const
{
    return !operator==(in_rhs);
}

