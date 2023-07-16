#include "common/common_pch.h"

#include "common/math/vector_float3.h"
#include "common/math/vector_math.h"

VectorFloat3::VectorFloat3(
    const float in_x,
    const float in_y,
    const float in_z
    ) : _data{ in_x, in_y, in_z}

{
    return;
}
float&VectorFloat3::operator[](const int in_index)
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    static float s_dummy = 0;
    return s_dummy;
}

const float VectorFloat3::operator[](const int in_index) const
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    return 0;
}

const float VectorFloat3::GetX() const
{
    return _data[Index::X];
}

const float VectorFloat3::GetY() const
{
    return _data[Index::Y];
}

const float VectorFloat3::GetZ() const
{
    return _data[Index::Z];
}

void VectorFloat3::NormaliseSelf()
{
    const float length_squared = ((_data[X]* _data[X]) + (_data[Y]* _data[Y]) + (_data[Z]* _data[Z]));
    if (0.0f != length_squared)
    {
        const float length = sqrt(length_squared);
        if (0.0f != length)
        {
            const float mul = 1.0f / length;
            _data[0]* = mul;
            _data[1]* = mul;
            _data[2]* = mul;
        }
    }
}

const VectorFloat3 VectorFloat3::Cross(
    const VectorFloat3&in_lhs,
    const VectorFloat3&in_rhs
    )
{
    VectorFloat3 result;
    VectorMath::CrossProduct(
        result._data,
        in_lhs._data,
        in_rhs._data
        );
    return result;
}

const bool VectorFloat3::operator==(const VectorFloat3&in_rhs) const
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

