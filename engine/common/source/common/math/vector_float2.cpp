#include "common/common_pch.h"

#include "common/math/vector_float2.h"
#include "common/math/vector_math.h"

const VectorFloat2 VectorFloat2::s_zero(0.0f, 0.0f);
const VectorFloat2 VectorFloat2::s_min(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
const VectorFloat2 VectorFloat2::s_max(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())

VectorFloat2::VectorFloat2(
    const float in_x,
    const float in_y
    ) : _data{ in_x, in_y}

{
    return;
}
float&VectorFloat2::operator[](const int in_index)
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    static float s_dummy = 0;
    return s_dummy;
}

const float VectorFloat2::operator[](const int in_index) const
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    return 0;
}

const float VectorFloat2::GetX() const
{
    return _data[Index::X];
}

const float VectorFloat2::GetY() const
{
    return _data[Index::Y];
}

void VectorFloat2::NormaliseSelf()
{
    const float length_squared = ((_data[X]* _data[X]) + (_data[Y]* _data[Y]));
    if (0.0f != length_squared)
    {
        const float length = sqrt(length_squared);
        if (0.0f != length)
        {
            const float mul = 1.0f / length;
            _data[0] *= mul;
            _data[1] *= mul;
        }
    }
}

const VectorFloat2 VectorFloat2::Cross(const VectorFloat2&in_subject)
{
    VectorFloat2 result(
        in_subject._data[Y],
        - (in_subject._data[X])
        );
    return result;
}

const bool VectorFloat2::operator==(const VectorFloat2&in_rhs) const
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

const bool VectorFloat2::operator!=(const VectorFloat2& in_rhs) const
{
    return !operator==(in_rhs);
}

const VectorFloat2 VectorFloat2::Min(const VectorFloat2& in_lhs, const VectorFloat2& in_rhs)
{
    return VectorFloat2(std::min(in_lhs.GetX(), in_rhs.GetX()),
        std::min(in_lhs.GetY(), in_rhs.GetY()));
}

const VectorFloat2 VectorFloat2::Max(const VectorFloat2& in_lhs, const VectorFloat2& in_rhs)
{
    return VectorFloat2(std::max(in_lhs.GetX(), in_rhs.GetX()),
        std::max(in_lhs.GetY(), in_rhs.GetY()));
}
