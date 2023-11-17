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
    static const UILayout FactoryContentMiddle();

    explicit UILayout(
        const UICoord& in_size_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_size_y = UICoord(UICoord::TSource::ParentY),
        const UICoord& in_attach_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_attach_y = UICoord(UICoord::TSource::ParentY),
        const UICoord& in_pivot_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_pivot_y = UICoord(UICoord::TSource::ParentY)
        //const bool in_shrink_width = false,
        //const bool in_shrink_height = false
        );

    const VectorInt2 GetSize(const VectorInt2& in_parent_size, const float in_ui_scale, const VectorInt2& in_content_size) const;
    //const VectorInt2 CalculateShrinkSize(const VectorInt2& in_layout_size, const VectorInt2& in_desired_size) const;

    const VectorInt2 GetAttach(const VectorInt2& in_size, const float in_ui_scale) const;
    const VectorInt2 GetPivot(const VectorInt2& in_parent_size, const float in_ui_scale, const VectorInt2& in_content_size) const;

    //const bool GetShrinkWidth() const { return _shrink_width; }
    //const bool GetShrinkHeight() const { return _shrink_height; }

    /// return true if _source wants content size for calculation
    const bool UsesContentSize() const;

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
    /// Data for how we calculate our size relative to parent
    /// Size of layout element relative to parent
    UICoord _data_size[2];
    /// Position on the layout element that is attached to the pivot on the parent
    UICoord _data_attach[2];
    /// The pivot on the parent layout that the layout element is attached to
    UICoord _data_pivot[2];

    // TODO: should we change the concept of shrink to use child size?
    //bool _shrink_width;
    //bool _shrink_height;

};

