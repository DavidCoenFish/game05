#include "common/common_pch.h"
#include "common/ui/ui_layout.h"

UILayout UILayout::FactoryFull()
{
    return UILayout(
        UICoord(UICoord::ParentSource::X, 1.0f, 0.0f),
        UICoord(UICoord::ParentSource::Y, 1.0f, 0.0f),
        UICoord(UICoord::ParentSource::X, 0.0f, 0.0f),
        UICoord(UICoord::ParentSource::Y, 0.0f, 0.0f),
        UICoord(UICoord::ParentSource::X, 0.0f, 0.0f),
        UICoord(UICoord::ParentSource::Y, 0.0f, 0.0f)
        );
}

UILayout::UILayout(
    const UICoord& in_size_x,
    const UICoord& in_size_y,
    const UICoord& in_attach_x,
    const UICoord& in_attach_y,
    const UICoord& in_pivot_x,
    const UICoord& in_pivot_y,
    const VectorFloat2& in_uv_scroll
    )
    : _uv_scroll(in_uv_scroll)
{
    _data_size[0] = in_size_x;
    _data_size[1] = in_size_y;
    _data_attach[0] = in_attach_x;
    _data_attach[1] = in_attach_y;
    _data_pivot[0] = in_pivot_x;
    _data_pivot[1] = in_pivot_y;
    return;
}

const VectorInt2 GetSize(const VectorInt2& in_parent_size, const float in_ui_scale) const
{
    return VectorInt2(
        _data_size[0].
        );
}

const bool UILayout::operator==(const UILayout& in_rhs) const
{
    if (_data_size[0] != in_rhs._data_size[0])
    {
        return false;
    }
    if (_data_size[1] != in_rhs._data_size[1])
    {
        return false;
    }

    if (_data_attach[0] != in_rhs._data_attach[0])
    {
        return false;
    }
    if (_data_attach[1] != in_rhs._data_attach[1])
    {
        return false;
    }

    if (_data_pivot[0] != in_rhs._data_pivot[0])
    {
        return false;
    }
    if (_data_pivot[1] != in_rhs._data_pivot[1])
    {
        return false;
    }

    if (_uv_scroll != in_rhs._uv_scroll)
    {
        return false;
    }

    return true;
}

const bool UILayout::operator!=(const UILayout& in_rhs) const
{
    return !operator==(in_rhs);
}

