#include "common/common_pch.h"
#include "common/text/text_cell.h"

TextCell::TextCell(
    const VectorInt2& in_width_height,
    const VectorInt2& in_bearing,
    const VectorFloat4& in_uv,
    const VectorFloat4& in_mask
    )
    : _width_height(in_width_height)
    , _bearing(in_bearing)
    , _uv(in_uv)
    , _mask(in_mask)
{
    // Nop
}