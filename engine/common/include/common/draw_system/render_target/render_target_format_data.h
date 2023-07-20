#pragma once
#include "common/math/vector_float4.h"

struct RenderTargetFormatData
{
public:
    explicit RenderTargetFormatData(
        const DXGI_FORMAT in_format = DXGI_FORMAT_UNKNOWN,
        const bool in_clear_on_set = false,
        const VectorFloat4& in_clear_color = VectorFloat4()
        );
    D3D12_CLEAR_VALUE MakeClearValue() const;

public:
    DXGI_FORMAT _format;
    bool _clear_on_set;
    VectorFloat4 _clear_color;
};
