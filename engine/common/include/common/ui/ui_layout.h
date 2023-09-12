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
        const VectorFloat2& in_uv_scroll = VectorFloat2()
        );

    const VectorInt2 GetSize(const VectorInt2& in_parent_size) const;

    const bool operator==(const UILayout& in_rhs) const;
    const bool operator!=(const UILayout& in_rhs) const;

private:
    // Data for how we calculate our size relative to parent
    UICoord _data_size[2];
    UICoord _data_attach[2];
    UICoord _data_pivot[2];

    // uv = abs(_uv_scroll), and use range [-1...1] wrapped when advancing _uv_scroll, to allow saw tooth animation
    // Scale update speed by desired size ratio to target size?
    VectorFloat2 _uv_scroll;
};

