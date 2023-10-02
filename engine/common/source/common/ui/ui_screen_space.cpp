#include "common/common_pch.h"
#include "common/ui/ui_screen_space.h"

UIScreenSpace::UIScreenSpace(
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
    : _pos(in_pos)
    , _uv(in_uv)
{
    // Nop
}

UIScreenSpace::~UIScreenSpace()
{
    // Nop
}

void UIScreenSpace::Set(
    const VectorFloat4& in_pos,
    const VectorFloat4& in_uv
    )
{
    if ((_pos == in_pos) &&
        (_uv == in_uv))
    {
        return;
    }
    _pos = in_pos;
    _uv = in_uv;

    return;
}
