#pragma once

#include "common/ui/ui_coord.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"

class UILayout
{
public:
    static const UILayout FactoryFull();
    static const UILayout FactoryParentMiddleQuater();
    //static const UILayout FactoryContentMiddle();
    //static const UILayout FactoryContentMiddleMargin(const float in_margin);

    enum class TAdjustmentType
    {
        None,
        Shrink,
        Grow
    };

    explicit UILayout(
        const UICoord& in_size_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_size_y = UICoord(UICoord::TSource::ParentY),
        const UICoord& in_attach_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_attach_y = UICoord(UICoord::TSource::ParentY),
        const UICoord& in_pivot_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_pivot_y = UICoord(UICoord::TSource::ParentY),
        const TAdjustmentType in_adjustment_type_x = TAdjustmentType::None,
        const TAdjustmentType in_adjustment_type_y = TAdjustmentType::None
        );

    /// Get the size of the layout element
    const VectorInt2 GetSize(const VectorInt2& in_reference_size, const float in_ui_scale) const;

    /// Where on the layout to attached to the pivot
    const VectorInt2 GetAttach(const VectorInt2& in_layout_size, const float in_ui_scale) const;

    /// Get the pivot on the parent to attach the layout element
    const VectorInt2 GetPivot(const VectorInt2& in_reference_size, const float in_ui_scale) const;

    /// return an adjusted layout size for the UILayout adjustment type
    const VectorInt2 AdjustLayoutSize(const VectorInt2& in_layout_size, const VectorInt2& in_desired_size) const;

    /// TODO: would it be better to just have accessors to set _data_attach and _data_pivot, and have SetSliderHorizontal... be helper functions?
    void SetSliderHorizontal(const float in_value);
    void SetSliderVertical(const float in_value);
    void SetScrollHorizontal(const VectorFloat2& in_value);
    void SetScrollVertical(const VectorFloat2& in_value);
    void SetTooltip(const VectorFloat2& in_tooltip_pos);
    void SetComboBoxDropdown(const VectorFloat4& in_combo_box_screen_pos);

    const bool operator==(const UILayout& in_rhs) const;
    const bool operator!=(const UILayout& in_rhs) const;

private:
    /// Coord data to calculate the size of layout element
    UICoord _data_size[2];
    /// Position on the layout element that is attached to the pivot on the parent
    UICoord _data_attach[2];
    /// The pivot on the parent layout that the layout element is attached to
    UICoord _data_pivot[2];

    /// how to adjust the x,y axis of the layout size against a desired size
    TAdjustmentType _adjustment_type[2];

};

