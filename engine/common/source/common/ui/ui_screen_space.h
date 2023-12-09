#pragma once

#include "common/math/vector_float4.h"

/// using render coordinates, -1,-1 is the lower left of the screen, 1,1 is the upper right
class UIScreenSpace
{
public:
    /// default constructor is a full screen with no clipping
    UIScreenSpace(
        const VectorFloat4& in_pos = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f),
        const VectorFloat4& in_clip = VectorFloat4(-1.0f, -1.0f, 1.0f, 1.0f)
        );
    ~UIScreenSpace();

    void Set(
        const VectorFloat4& in_pos,
        const VectorFloat4& in_clip
        );

    void Update(
        const UIScreenSpace& in_parent,
        const VectorFloat4& in_pos,
        const VectorFloat4& in_uv
        );

    const VectorFloat4& GetPosRef() const { return _pos; }
    const VectorFloat4& GetClipRef() const { return _clip; }

private:
    /// screen space pos we would end up at, ie, flattenened
    VectorFloat4 _pos;
    /// if each UISceenSpace clipped content at bounds, what would our screen space coverage be
    VectorFloat4 _clip;

};
