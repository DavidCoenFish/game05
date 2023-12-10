#include "common/common_pch.h"

#include "common/math/vector_float4.h"
#include "common/math/vector_float2.h"

const VectorFloat4 VectorFloat4::s_zero(0.0f, 0.0f, 0.0f, 0.0f);
const VectorFloat4 VectorFloat4::s_white(1.0f, 1.0f, 1.0f, 1.0f);
const VectorFloat4 VectorFloat4::s_render_coordinates_full(-1.0f, -1.0f, 1.0f, 1.0f);

VectorFloat4::VectorFloat4(
    const float in_x,
    const float in_y,
    const float in_z,
    const float in_w
    ) : _data{ in_x, in_y, in_z, in_w}
{
    return;
}

float& VectorFloat4::operator[](const int in_index)
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    static float s_dummy = 0;
    return s_dummy;
}

const float VectorFloat4::operator[](const int in_index) const
{
    if ((0 <= in_index) && (in_index < Index::Count))
    {
        return _data[in_index];
    }
    return 0;
}

const float VectorFloat4::GetX() const
{
    return _data[Index::X];
}

const float VectorFloat4::GetY() const
{
    return _data[Index::Y];
}

const float VectorFloat4::GetZ() const
{
    return _data[Index::Z];
}

const float VectorFloat4::GetW() const
{
    return _data[Index::W];
}

const bool VectorFloat4::operator==(const VectorFloat4& in_rhs) const
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

const bool VectorFloat4::operator!=(const VectorFloat4& in_rhs) const
{
    return !operator==(in_rhs);
}


VectorFloat4& VectorFloat4::operator=(const VectorFloat4& in_rhs)
{
    for (int index = 0; index < Index::Count; ++index)
    {
        _data[index] = in_rhs._data[index];
    }
    return (*this);
}

const bool VectorFloat4::Inside(
    const VectorFloat2& in_value
    ) const
{
    return ((_data[0] <= in_value[0]) &&
            (_data[1] <= in_value[1]) &&
            (in_value[0] < _data[2]) &&
            (in_value[1] < _data[3]));
}
