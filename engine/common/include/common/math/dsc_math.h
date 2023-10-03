#pragma once

class VectorFloat4;

namespace DscMath
{
    const int ScaleInt(const int in_value, const float in_scale);
    //constexpr int ScaleInt(constexpr int in_value, constexpr float in_scale);

    const float ConvertZeroOneToNegativeOneOne(const float in_value);
    const VectorFloat4 ConvertZeroOneToNegativeOneOne(const VectorFloat4& in_value);

    const float ConvertNegativeOneOneToZeroOne(const float in_value);
    const VectorFloat4 ConvertNegativeOneOneToZeroOne(const VectorFloat4& in_value);

}
