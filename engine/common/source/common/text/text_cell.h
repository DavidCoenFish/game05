#pragma once

#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class TextCell
{
public:
    TextCell(
        const VectorInt2& in_width_height,
        const VectorInt2& in_bearing,
        const VectorFloat4& in_uv,
        const VectorFloat4& in_mask
        );

    VectorInt2 GetWidthHeight() const { return _width_height; }
    VectorInt2 GetBearing() const { return _bearing; }
    VectorFloat4 GetUV() const { return _uv; }
    VectorFloat4 GetMask() const { return _mask; }

private:
    VectorInt2 _width_height;
    VectorInt2 _bearing;
    VectorFloat4 _uv;
    VectorFloat4 _mask;

};
