#include "common/common_pch.h"

#include "common/math/dsc_math.h"
#include "common/math/vector_float4.h"
#include "common/macro.h"

const int DscMath::ScaleInt(const int in_value, const float in_scale)
{
    return static_cast<int>(round(in_value * in_scale));
}

const float DscMath::ConvertZeroOneToNegativeOneOne(const float in_value)
{
    return (-1.0f + (2.0f * in_value));
}

const VectorFloat4 DscMath::ConvertZeroOneToNegativeOneOne(const VectorFloat4& in_value)
{
    return VectorFloat4(
        ConvertZeroOneToNegativeOneOne(in_value[0]),
        ConvertZeroOneToNegativeOneOne(in_value[1]),
        ConvertZeroOneToNegativeOneOne(in_value[2]),
        ConvertZeroOneToNegativeOneOne(in_value[3])
        );
}

const float DscMath::ConvertNegativeOneOneToZeroOne(const float in_value)
{
    return ((in_value + 1.0f) / 2.0f);
}

const VectorFloat4 DscMath::ConvertNegativeOneOneToZeroOne(const VectorFloat4& in_value)
{
    return VectorFloat4(
        ConvertNegativeOneOneToZeroOne(in_value[0]),
        ConvertNegativeOneOneToZeroOne(in_value[1]),
        ConvertNegativeOneOneToZeroOne(in_value[2]),
        ConvertNegativeOneOneToZeroOne(in_value[3])
        );
}
