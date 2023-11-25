#include "common/common_pch.h"
#include "common/ui/ui_layout.h"
#include "common/math/dsc_math.h"

const UILayout UILayout::FactoryFull()
{
    return UILayout(
        UICoord(UICoord::TSource::ParentX, 1.0f, 0.0f),
        UICoord(UICoord::TSource::ParentY, 1.0f, 0.0f),
        UICoord(UICoord::TSource::ParentX, 0.0f, 0.0f),
        UICoord(UICoord::TSource::ParentY, 0.0f, 0.0f),
        UICoord(UICoord::TSource::ParentX, 0.0f, 0.0f),
        UICoord(UICoord::TSource::ParentY, 0.0f, 0.0f)
        );
}

const UILayout UILayout::FactoryParentMiddleQuater()
{
    return UILayout(
        UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
        UICoord(UICoord::TSource::ParentY, 0.5f, 0.0f),
        UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
        UICoord(UICoord::TSource::ParentY, 0.5f, 0.0f),
        UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
        UICoord(UICoord::TSource::ParentY, 0.5f, 0.0f)
        );
}

UILayout::UILayout(
    const UICoord& in_size_x,
    const UICoord& in_size_y,
    const UICoord& in_attach_x,
    const UICoord& in_attach_y,
    const UICoord& in_pivot_x,
    const UICoord& in_pivot_y,
    const TAdjustmentType in_adjustment_type_x,
    const TAdjustmentType in_adjustment_type_y,
    const VectorInt4& in_desired_margin
    )
    : _desired_margin(in_desired_margin)
{
    _data_size[0] = in_size_x;
    _data_size[1] = in_size_y;
    _data_attach[0] = in_attach_x;
    _data_attach[1] = in_attach_y;
    _data_pivot[0] = in_pivot_x;
    _data_pivot[1] = in_pivot_y;
    _adjustment_type[0] = in_adjustment_type_x;
    _adjustment_type[1] = in_adjustment_type_y;
    return;
}

const VectorInt2 UILayout::GetSize(const VectorInt2& in_parent_size, const float in_ui_scale) const
{
    return VectorInt2(
        _data_size[0].Calculate(in_parent_size, in_ui_scale),
        _data_size[1].Calculate(in_parent_size, in_ui_scale)
        );
}

//const VectorInt2 UILayout::CalculateShrinkSize(const VectorInt2& in_layout_size, const VectorInt2& in_desired_size) const
//{
//    return VectorInt2(
//        _shrink_width ? std::min(in_layout_size.GetX(), in_desired_size.GetX()) : in_layout_size.GetX(),
//        _shrink_height ? std::min(in_layout_size.GetY(), in_desired_size.GetY()) : in_layout_size.GetY()
//        );
//}

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
    _data_pivot[0] = UICoord(UICoord::TSource::ParentX, in_value);
    //_data_attach[0] = UICoord(UICoord::TSource::ParentX, 1.0f - in_value);
    _data_attach[0] = UICoord(UICoord::TSource::ParentX, in_value);
    return;
}

void UILayout::SetSliderVertical(const float in_value)
{
    _data_pivot[1] = UICoord(UICoord::TSource::ParentY, in_value);
    //_data_attach[1] = UICoord(UICoord::TSource::ParentY, 1.0f - in_value);
    _data_attach[1] = UICoord(UICoord::TSource::ParentY, in_value);
    return;
}

void UILayout::SetScrollHorizontal(const VectorFloat2& in_value)
{
    _data_size[0] = UICoord(UICoord::TSource::ParentX, in_value[1] - in_value[0]);
    _data_pivot[0] = UICoord(UICoord::TSource::ParentX, in_value[0]);
    _data_attach[0] = UICoord(UICoord::TSource::ParentX, 0.0f);
}

void UILayout::SetScrollVertical(const VectorFloat2& in_value)
{
    _data_size[1] = UICoord(UICoord::TSource::ParentY, in_value[1] - in_value[0]);
    _data_pivot[1] = UICoord(UICoord::TSource::ParentY, 1.0f - in_value[0]);
    _data_attach[1] = UICoord(UICoord::TSource::ParentY, 1.0f);
}

void UILayout::SetTooltip(const VectorFloat2& in_tooltip_pos)
{
    _data_pivot[0] = UICoord(UICoord::TSource::ParentX, DscMath::ConvertNegativeOneOneToZeroOne(in_tooltip_pos[0]));
    _data_pivot[1] = UICoord(UICoord::TSource::ParentY, DscMath::ConvertNegativeOneOneToZeroOne(in_tooltip_pos[1]));
    _data_attach[0] = UICoord(UICoord::TSource::ParentX, 0.5f);
    _data_attach[1] = UICoord(UICoord::TSource::ParentY, 0.0f);
}

void UILayout::SetComboBoxDropdown(const VectorFloat4& in_combo_box_screen_pos)
{
    const float width = (in_combo_box_screen_pos[2] - in_combo_box_screen_pos[0]) * 0.5f;
    const float mid_x = (in_combo_box_screen_pos[2] + in_combo_box_screen_pos[0]) * 0.5f;
    _data_size[0] = UICoord(UICoord::TSource::ParentX, width);
    _data_pivot[0] = UICoord(UICoord::TSource::ParentX, DscMath::ConvertNegativeOneOneToZeroOne(mid_x));
    _data_pivot[1] = UICoord(UICoord::TSource::ParentY, DscMath::ConvertNegativeOneOneToZeroOne(in_combo_box_screen_pos[1]));
    _data_attach[0] = UICoord(UICoord::TSource::ParentX, 0.5f);
    _data_attach[1] = UICoord(UICoord::TSource::ParentY, 1.0f);
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

    if (_adjustment_type[0] != in_rhs._adjustment_type[0])
    {
        return false;
    }

    if (_adjustment_type[1] != in_rhs._adjustment_type[1])
    {
        return false;
    }

    return true;
}

const bool UILayout::operator!=(const UILayout& in_rhs) const
{
    return !operator==(in_rhs);
}
