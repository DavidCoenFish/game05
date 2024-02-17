#include "common/common_pch.h"

#include "common/math/dsc_math.h"
#include "common/math/vector_float4.h"
#include "common/macro.h"

const bool DscMath::AlmostEqual(const float in_lhs, const float in_rhs, const float in_epsilon)
{
    return (abs(in_lhs - in_rhs) <= in_epsilon);
}

// https://en.wikibooks.org/wiki/Algorithms/Distance_approximations
const float DscMath::ApproximateDistance(const float in_dx, const float in_dy)
{
    const float dx = abs(in_dx);
    const float dy = abs(in_dy);
    if (dx < dy)
    {
        return (0.41f * dx) + (0.941246f * dy);
    }

    return (0.41f * dy) + (0.941246f * dx);
}

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

const int DscMath::ConvertColourToInt(const VectorFloat4& in_value)
{
    const int result = 
        ConvertFloatToByte(in_value.GetX()) |
        (ConvertFloatToByte(in_value.GetY()) << 8) |
        (ConvertFloatToByte(in_value.GetZ()) << 16) |
        (ConvertFloatToByte(in_value.GetW()) << 24);
    return result;
}

const int DscMath::ConvertFloatToByte(const float in_value)
{
#if 1
    // mul by 256.0 and take the floor to get better mapping of range? converting a coin toss, you would multiply by 2?
    const int result = std::max(0, std::min(255, static_cast<int>(floor(in_value * 256.0f))));
#else
    const int result = std::max(0, std::min(255, static_cast<int>(round(in_value * 255.0f))));
#endif
    return result;
}

