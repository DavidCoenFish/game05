#pragma once

#include "common/ui/ui_coord.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"

/// trying to allow for tooltip to have outer element shrink to content but still have a margin
class UILayout
{
public:
    static const UILayout FactoryFull();
    static const UILayout FactoryParentMiddleQuater();
    //static const UILayout FactoryContentMiddle();
    //static const UILayout FactoryContentMiddleMargin(const float in_margin);

    enum class TAdjustmentType
    {
        GrowTextureToLayout, // if texture is smaller than layout, inflate texture. if texture is bigger, no change to texture size and it auto scrolls around inside layout
        //MatchTextureToLayout,
        ShrinkLayoutToTexture, //ok to reduce the layout size to match the texture size for the given axis
        MatchLayoutToTexture // ok to shrink or grow the layout size to match the texture (desired) size for the given axis
    };

    explicit UILayout(
        const UICoord& in_layout_size_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_layout_size_y = UICoord(UICoord::TSource::ParentY),
        const VectorFloat2& in_layout_space_bias = VectorFloat2::s_zero,
        const TAdjustmentType in_adjustment_type_x = UILayout::TAdjustmentType::GrowTextureToLayout,
        const TAdjustmentType in_adjustment_type_y = UILayout::TAdjustmentType::GrowTextureToLayout,
        const VectorInt4& in_texture_margin = VectorInt4::s_zero, // left, top, right, bottom
        const VectorFloat2& in_adjustment_space_bias = VectorFloat2::s_zero
        );

    // left, top, right, bottom
    const VectorInt4& GetTextureMarginRef() const { return _texture_margin; }

    /// Get the size of the layout element, not including the margin (margin to be subtracted from this to get the active space)
    const VectorInt2 CalculateLayoutSize(const VectorInt2& in_parent_window_size, const float in_ui_scale) const;

    /// not including the margin (which is internal) what is the offset from the bottom left for this element given our layout bias
    const VectorInt2 CalculateLayoutOffset(const VectorInt2& in_parent_window_size, const VectorInt2& in_layout_size) const;

    /// For children of the layout element, what offset for the margin
    const VectorInt2 CalculateMarginOffset(const float in_ui_scale) const;

    /// calculate an adjusted layout and texture size for our adjustment type, 
    void Finalise(
        VectorInt2& out_layout_size,
        VectorInt2& out_texture_size,
        VectorInt2& out_adjust_offset,
        const VectorInt2& in_layout_size,
        const VectorInt2& in_texture_size
        ) const;

    const VectorInt2 AddMargin(
        const VectorInt2& in_size,
        const float in_ui_scale
        ) const;
    const VectorInt2 SubtractMargin(
        const VectorInt2& in_size,
        const float in_ui_scale
        ) const;

    /// Return true if either axis adjustment modifies the layout size, used for knowing if component change can modify layout UIHierarchyNodeChild::ApplyComponent
    const bool GetAdjustmentModifiesLayout() const;

    /// TODO: would it be better to just have accessors to set _data_attach and _data_pivot, and have SetSliderHorizontal... be helper functions?
    //void SetSliderHorizontal(const float in_value);
    //void SetSliderVertical(const float in_value);
    //void SetScrollHorizontal(const VectorFloat2& in_value);
    //void SetScrollVertical(const VectorFloat2& in_value);
    //void SetTooltip(const VectorFloat2& in_tooltip_pos);
    //void SetComboBoxDropdown(const VectorFloat4& in_combo_box_screen_pos);

    /// return true if value changes
    const bool SetLayoutSpaceBias(const VectorFloat2& in_layout_space_bias);

    const bool operator==(const UILayout& in_rhs) const;
    const bool operator!=(const UILayout& in_rhs) const;

private:
    /// Coord data to calculate the size of layout element
    UICoord _layout_size[2];
    /// how much of the parents space is around the layout element [left - right, bottom - top]
    /// replaces _layout_pivot_ratio and _parent_window_attach_ratio
    VectorFloat2 _layout_space_bias;

    /// how to adjust the x,y axis of the layout/texture size
    TAdjustmentType _adjustment_type[2];
    /// how much of the possition offset caused by the adjustment is returned as layout offset
    VectorFloat2 _adjustment_space_bias;

    /// amount to inflate the texture size with
    /// left, top, right, bottom
    VectorInt4 _texture_margin;

};

