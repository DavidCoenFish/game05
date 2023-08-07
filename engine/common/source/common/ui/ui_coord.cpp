#include "common/common_pch.h"
#include "common/ui/ui_coord.h"

UICoord::UICoord(
    const float in_ratio,
    const float in_em_offset
    )
    : _ratio(in_ratio)
    , _em_offset(in_em_offset)
{
    // Nop
}

UICoord::~UICoord()
{
    // Nop
}

const float UICoord::Calculate(
    const float in_parent_dim,
    const float in_ui_scale // pixels per em
    ) const
{
    return ((in_parent_dim * _ratio) + (_em_offset * in_ui_scale));
}
