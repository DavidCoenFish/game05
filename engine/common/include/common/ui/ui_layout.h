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
        GrowTextureToLayout, // if texture is smaller than layout, inflate texture
        ShrinkLayoutToTexture, //ok to reduce the layout size to match the texture size for the given axis
        ShrinkOrGrowLayoutToTexture // ok to shrink or grow the layout size to match the desired size for the given axis
    };

    explicit UILayout(
        const UICoord& in_size_x = UICoord(UICoord::TSource::ParentX),
        const UICoord& in_size_y = UICoord(UICoord::TSource::ParentY),
        const VectorFloat2& in_layout_attach_ratio = VectorFloat2::s_zero,
        const VectorFloat2& in_parent_window_pivot_ratio = VectorFloat2::s_zero,
        const TAdjustmentType in_adjustment_type_x = TAdjustmentType::GrowTextureToLayout,
        const TAdjustmentType in_adjustment_type_y = TAdjustmentType::GrowTextureToLayout,
        const VectorInt4& in_texture_margin = VectorInt4::s_zero // left, top, right, bottom
        );

    const VectorInt4& GetTextureMarginRef() const { return _texture_margin; }

    /// Get the size of the layout element
    const VectorInt2 GetLayoutSize(const VectorInt2& in_parent_window_size, const float in_ui_scale) const;

    /// Where on the layout to attached to the pivot
    const VectorInt2 GetLayoutAttach(const VectorInt2& in_layout_size) const;

    /// Get the pivot on the parent to attach the layout element
    const VectorInt2 GetParentWindowPivot(const VectorInt2& in_parent_window_size, const VectorInt2& in_parent_offset) const;

    /// We don't append the margin to the texture (inflating the texture) we want to deal with this at the geometry level
    ///  does this mean we subtract the margin from the layout size?
    //const VectorInt2 ApplyTextureMargin(const VectorInt2& in_texture_size, const float in_ui_scale) const;

    /// calculate an adjusted layout and texture size for our adjustment type, 
    /// texture size is increaced to at least match the layout size, ie, it can be bigger, but it may not be smaller
    void Finalise(
        VectorInt2& out_layout_size,
        VectorInt2& out_texture_size,
        VectorInt2& out_layout_offset,
        const VectorInt2& in_layout_size,
        const VectorInt2& in_texture_size,
        const VectorInt2& in_parent_offset
        ) const;

    // shring the window by texture margin, also nudge the window offset
    const VectorInt2 ApplyMargin(
        const VectorInt2& in_size,
        const float in_ui_scale
        ) const;

    // Merge Finalise and ApplyMargin? or is that up to the component to deal with, ie, stack

    /// Return true if either axis adjustment modifies the layout size
    const bool GetAdjustmentModifiesLayout() const;

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
    UICoord _size[2];
    /// Position on the layout element that is attached to the pivot on the parent
    VectorFloat2 _layout_attach_ratio;
    /// The pivot on the parent layout that the layout element is attached to
    VectorFloat2 _parent_window_pivot_ratio;
    /// how to adjust the x,y axis of the layout size against a texture size
    TAdjustmentType _adjustment_type[2];
    /// amount to inflate the texture size with
    VectorInt4 _texture_margin;

};

