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
    const bool in_shrink_width,
    const bool in_shrink_height
    )
    : _shrink_width(in_shrink_width)
    , _shrink_height(in_shrink_height)
{
    _data_size[0] = in_size_x;
    _data_size[1] = in_size_y;
    _data_attach[0] = in_attach_x;
    _data_attach[1] = in_attach_y;
    _data_pivot[0] = in_pivot_x;
    _data_pivot[1] = in_pivot_y;
    return;
}

const VectorInt2 UILayout::GetSize(const VectorInt2& in_parent_size, const float in_ui_scale) const
{
    return VectorInt2(
        _data_size[0].Calculate(in_parent_size, in_ui_scale),
        _data_size[1].Calculate(in_parent_size, in_ui_scale)
        );
}

const VectorInt2 UILayout::CalculateShrinkSize(const VectorInt2& in_layout_size, const VectorInt2& in_desired_size) const
{
    return VectorInt2(
        _shrink_width ? std::min(in_layout_size.GetX(), in_desired_size.GetX()) : in_layout_size.GetX(),
        _shrink_height ? std::min(in_layout_size.GetY(), in_desired_size.GetY()) : in_layout_size.GetY()
        );
}

const VectorInt2 UILayout::GetAttach(const VectorInt2& in_size, const float in_ui_scale) const
{
    return VectorInt2(
        _data_attach[0].Calculate(in_size, in_ui_scale),
        _data_attach[1].Calculate(in_size, in_ui_scale)
        );
}

const VectorInt2 UILayout::GetPivot(const VectorInt2& in_parent_size, const float in_ui_scale) const
{
    return VectorInt2(
        _data_pivot[0].Calculate(in_parent_size, in_ui_scale),
        _data_pivot[1].Calculate(in_parent_size, in_ui_scale)
        );
}

void UILayout::SetSliderHorizontal(const float in_value)
{
    _data_pivot[0] = UICoord(UICoord::ParentSource::X, in_value);
    //_data_attach[0] = UICoord(UICoord::ParentSource::X, 1.0f - in_value);
    _data_attach[0] = UICoord(UICoord::ParentSource::X, in_value);
    return;
}

void UILayout::SetSliderVertical(const float in_value)
{
    _data_pivot[1] = UICoord(UICoord::ParentSource::Y, in_value);
    //_data_attach[1] = UICoord(UICoord::ParentSource::Y, 1.0f - in_value);
    _data_attach[1] = UICoord(UICoord::ParentSource::Y, in_value);
    return;
}

void UILayout::SetScrollHorizontal(const VectorFloat2& in_value)
{
    _data_size[0] = UICoord(UICoord::ParentSource::X, in_value[1] - in_value[0]);
    _data_pivot[0] = UICoord(UICoord::ParentSource::X, in_value[0]);
    _data_attach[0] = UICoord(UICoord::ParentSource::X, 0.0f);
}

void UILayout::SetScrollVertical(const VectorFloat2& in_value)
{
    _data_size[1] = UICoord(UICoord::ParentSource::Y, in_value[1] - in_value[0]);
    _data_pivot[1] = UICoord(UICoord::ParentSource::Y, 1.0f - in_value[0]);
    _data_attach[1] = UICoord(UICoord::ParentSource::Y, 1.0f);
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

    if (_shrink_width != in_rhs._shrink_width)
    {
        return false;
    }

    if (_shrink_height != in_rhs._shrink_height)
    {
        return false;
    }

    return true;
}

const bool UILayout::operator!=(const UILayout& in_rhs) const
{
    return !operator==(in_rhs);
}
