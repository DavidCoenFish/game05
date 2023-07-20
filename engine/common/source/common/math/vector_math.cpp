#include "common/common_pch.h"

#include "common/math/vector_math.h"

void VectorMath::CrossProduct(
    float in_output[3],
    const float in_lhs[3],
    const float in_rhs[3]
    )
{
    // left and right depend on how we lable axis
#if 1
    in_output[0] = (in_lhs[1]* in_rhs[2]) - (in_lhs[2]* in_rhs[1]);
    in_output[1] = (in_lhs[2]* in_rhs[0]) - (in_lhs[0]* in_rhs[2]);
    in_output[2] = (in_lhs[0]* in_rhs[1]) - (in_lhs[1]* in_rhs[0]);
#else
    in_output[0] = (in_lhs[2] * in_rhs[1]) - (in_lhs[1] * in_rhs[2]);
    in_output[1] = (in_lhs[0] * in_rhs[2]) - (in_lhs[2] * in_rhs[0]);
    in_output[2] = (in_lhs[1] * in_rhs[0]) - (in_lhs[0] * in_rhs[1]);
#endif
    return;
}

