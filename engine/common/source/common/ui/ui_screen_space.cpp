#include "common/common_pch.h"
#include "common/ui/ui_screen_space.h"
#include "common/math/dsc_math.h"
#include "common/math/vector_float2.h"

namespace
{
    const VectorFloat4 CalculatePos(
        const VectorFloat4& in_parent_pos, 
        const VectorFloat4& in_geometry_pos, 
        const VectorFloat4& in_geometry_uv
        )
    {
        const VectorFloat4 zero_one_parent_pos = DscMath::ConvertNegativeOneOneToZeroOne(in_parent_pos);
        const VectorFloat4 zero_one_geometry_pos = DscMath::ConvertNegativeOneOneToZeroOne(in_geometry_pos);

        const VectorFloat2 uv_length(
            in_geometry_uv[2] - in_geometry_uv[0],
            in_geometry_uv[1] - in_geometry_uv[3]
            );
        const VectorFloat2 zero_one_geometry_length(
            (zero_one_geometry_pos[2] - zero_one_geometry_pos[0]) / uv_length[0],
            (zero_one_geometry_pos[3] - zero_one_geometry_pos[1]) / uv_length[1]
            );

        const VectorFloat2 zero_one_geometry_pivot(
            zero_one_geometry_pos[0] - (zero_one_geometry_pos[0] * in_geometry_uv[0]),
            zero_one_geometry_pos[1] - (zero_one_geometry_pos[1] * (1.0 - in_geometry_uv[1]))
            );

        const VectorFloat2 zero_one_new_pivot(
            zero_one_parent_pos[0] + ((zero_one_parent_pos[2] - zero_one_parent_pos[0]) * zero_one_geometry_pivot[0]),
            zero_one_parent_pos[1] + ((zero_one_parent_pos[3] - zero_one_parent_pos[1]) * zero_one_geometry_pivot[1])
            );

        const VectorFloat2 zero_one_new_end(
            zero_one_new_pivot[0] + ((zero_one_parent_pos[2] - zero_one_parent_pos[0]) * zero_one_geometry_length[0]),
            zero_one_new_pivot[1] + ((zero_one_parent_pos[3] - zero_one_parent_pos[1]) * zero_one_geometry_length[1])
            );

        const VectorFloat4 result(
            DscMath::ConvertZeroOneToNegativeOneOne(zero_one_new_pivot[0]),
            DscMath::ConvertZeroOneToNegativeOneOne(zero_one_new_pivot[1]),
            DscMath::ConvertZeroOneToNegativeOneOne(zero_one_new_end[0]),
            DscMath::ConvertZeroOneToNegativeOneOne(zero_one_new_end[1])
            );

        return result;
    }

    const VectorFloat4 CalculateClip(
        const VectorFloat4& in_parent_clip, 
        const VectorFloat4& in_pos
        )
    {
        VectorFloat4 result(
            std::max(in_parent_clip[0], in_pos[0]),
            std::max(in_parent_clip[1], in_pos[1]),
            std::min(in_parent_clip[2], in_pos[2]),
            std::min(in_parent_clip[3], in_pos[3])
            );
        result[0] = std::min(result[0], result[2]);
        result[1] = std::min(result[1], result[3]);

        return result;
    }

};

UIScreenSpace::UIScreenSpace(
    const VectorFloat4& in_pos,
    const VectorFloat4& in_clip
    )
    : _pos(in_pos)
    , _clip(in_clip)
{
    // Nop
}

UIScreenSpace::~UIScreenSpace()
{
    // Nop
}

void UIScreenSpace::Set(
    const VectorFloat4& in_pos,
    const VectorFloat4& in_clip
    )
{
    if ((_pos == in_pos) &&
        (_clip == in_clip))
    {
        return;
    }
    _pos = in_pos;
    _clip = in_clip;

    return;
}

void UIScreenSpace::Update(
    const UIScreenSpace& in_parent,
    const VectorFloat4& in_geometry_pos,
    const VectorFloat4& in_geometry_uv
    )
{
    VectorFloat4 pos = CalculatePos(in_parent.GetPosRef(), in_geometry_pos, in_geometry_uv);
    VectorFloat4 clip = CalculateClip(in_parent.GetClipRef(), pos); 

    Set(pos, clip);

    return;
}


