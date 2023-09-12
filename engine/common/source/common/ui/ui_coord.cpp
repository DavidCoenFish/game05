#include "common/common_pch.h"
#include "common/ui/ui_coord.h"

#include "common/math/vector_float2.h"
#include "common/math/vector_int2.h"

UICoord::UICoord(
    const ParentSource in_parent_source,
    const float in_ratio,
    const float in_em_offset
    )
    : _parent_source(in_parent_source)
    , _ratio(in_ratio)
    , _em_offset(in_em_offset)
{
    // Nop
}

UICoord::~UICoord()
{
    // Nop
}

const float UICoord::Calculate(
    const VectorFloat2& in_parent_size,
    //const VectorInt2& in_parent_size,
    const float in_ui_scale
    ) const
{
    float parent_dim = 0.0f;
    switch(_parent_source)
    {
    default:
        break;
    case ParentSource::X:
        parent_dim = in_parent_size.GetX();
        break;
    case ParentSource::Y:
        parent_dim = in_parent_size.GetY();
        break;
    case ParentSource::Max:
        parent_dim = std::max(in_parent_size.GetX(), in_parent_size.GetY());
        break;
    case ParentSource::Min:
        parent_dim = std::min(in_parent_size.GetX(), in_parent_size.GetY());
        break;
    }

    return ((parent_dim * _ratio) + (_em_offset * in_ui_scale));
}

const bool UICoord::operator==(const UICoord& in_rhs) const
{
    if (_parent_source != in_rhs._parent_source)
    {
        return false;
    }
    if (_ratio != in_rhs._ratio)
    {
        return false;
    }
    if (_em_offset != in_rhs._em_offset)
    {
        return false;
    }

    return true;
}

const bool UICoord::operator!=(const UICoord& in_rhs) const
{
    return !operator==(in_rhs);
}
