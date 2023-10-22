#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    Pixel result;
    result._colour = texel * _tint;
    //result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);

    return result;
}