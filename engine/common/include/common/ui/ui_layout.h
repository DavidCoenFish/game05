#pragma once

#include "common/ui/ui_coord.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"

class UILayout
{
public:
    static UILayout FactoryFull();
    explicit UILayout(
        const UICoord& in_size_x = UICoord(UICoord::ParentSource::X),
        const UICoord& in_size_y = UICoord(UICoord::ParentSource::Y),
        const UICoord& in_attach_x = UICoord(UICoord::ParentSource::X),
        const UICoord& in_attach_y = UICoord(UICoord::ParentSource::Y),
        const UICoord& in_pivot_x = UICoord(UICoord::ParentSource::X),
        const UICoord& in_pivot_y = UICoord(UICoord::ParentSource::Y),
        const bool in_shrink_width = false,
        const bool in_shrink_height = false
        );

    const VectorInt2 GetSize(const VectorInt2& in_parent_size, const float in_ui_scale) const;
    const VectorInt2 CalculateShrinkSize(const VectorInt2& in_layout_size, const VectorInt2& in_desired_size) const;

    const VectorInt2 GetAttach(const VectorInt2& in_size, const float in_ui_scale) const;
    const VectorInt2 GetPivot(const VectorInt2& in_parent_size, const float in_ui_scale) const;

    const bool GetShrinkWidth() const { return _shrink_width; }
    const bool GetShrinkHeight() const { return _shrink_height; }

    void SetSliderHorisontal(const float in_value);
    void SetSliderVertical(const float in_value);

    const bool operator==(const UILayout& in_rhs) const;
    const bool operator!=(const UILayout& in_rhs) const;

private:
    // Data for how we calculate our size relative to parent
    UICoord _data_size[2];
    UICoord _data_attach[2];
    UICoord _data_pivot[2];

    bool _shrink_width;
    bool _shrink_height;

};

