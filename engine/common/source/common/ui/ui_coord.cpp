#include "common/common_pch.h"
#include "common/ui/ui_coord.h"

#include "common/math/vector_float2.h"
#include "common/math/vector_int2.h"

UICoord::UICoord(
    const TSource in_source,
    const float in_ratio,
    const float in_em_offset
    )
    : _source(in_source)
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
    const float in_ui_scale,
    const VectorFloat2& in_content_size
    ) const
{
    float dim = 0.0f;
    switch(_source)
    {
    default:
        break;
    case TSource::ParentX:
        dim = in_parent_size.GetX();
        break;
    case TSource::ParentY:
        dim = in_parent_size.GetY();
        break;
    case TSource::ParentMax:
        dim = std::max(in_parent_size.GetX(), in_parent_size.GetY());
        break;
    case TSource::ParentMin:
        dim = std::min(in_parent_size.GetX(), in_parent_size.GetY());
        break;
    case TSource::ParentRatio_4_3_X:
        dim = std::min(
            in_parent_size.GetX(),
            (in_parent_size.GetY() / 3.0f) * 4.0f
            );
        break;
    case TSource::ParentRatio_4_3_Y:
        dim = std::min(
            (in_parent_size.GetX() / 4.0f) * 3.0f, 
            in_parent_size.GetY()
            );
        break;
    case TSource::ContentX:
        dim = in_content_size.GetX();
        break;
    case TSource::ContentY:
        dim = in_content_size.GetY();
        break;
    }

    return ((dim * _ratio) + (_em_offset * in_ui_scale));
}

const int UICoord::Calculate(
    const VectorInt2& in_parent_size,
    const float in_ui_scale,
    const VectorInt2& in_content_size
    ) const
{
    int dim = 0;
    switch(_source)
    {
    default:
        break;
    case TSource::ParentX:
        dim = in_parent_size.GetX();
        break;
    case TSource::ParentY:
        dim = in_parent_size.GetY();
        break;
    case TSource::ParentMax:
        dim = std::max(in_parent_size.GetX(), in_parent_size.GetY());
        break;
    case TSource::ParentMin:
        dim = std::min(in_parent_size.GetX(), in_parent_size.GetY());
        break;
    case TSource::ParentRatio_4_3_X:
        dim = std::min(
            in_parent_size.GetX(),
            (in_parent_size.GetY() * 4) / 3
            );
        break;
    case TSource::ParentRatio_4_3_Y:
        dim = std::min(
            (in_parent_size.GetX() * 3) / 4, 
            in_parent_size.GetY()
            );
        break;
    case TSource::ContentX:
        dim = in_content_size.GetX();
        break;
    case TSource::ContentY:
        dim = in_content_size.GetY();
        break;
    }

    return static_cast<int>(round((dim * _ratio) + (_em_offset * in_ui_scale)));
}

const bool UICoord::operator==(const UICoord& in_rhs) const
{
    if (_source != in_rhs._source)
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

const bool UICoord::UsesContentSize() const
{
    switch(_source)
    {
    default:
        break;
    case TSource::ContentX:
    case TSource::ContentY:
        return true;
    }
    return false;
}

