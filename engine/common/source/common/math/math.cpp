#include "common/common_pch.h"

#include "common/math/math.h"
#include "common/macro.h"

const int Math::ScaleInt(const int in_value, const float in_scale)
{
    return static_cast<int>(round(in_value * in_scale));
}
