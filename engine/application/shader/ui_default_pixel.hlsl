#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"

cbuffer ConstantBufferBlock : register(b0)
{
    float4 _tint;
};

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    Pixel result;
    result._colour = texel * _tint;

    //result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);
    //result = foreground + (1 - foreground.alpha) * background
    //result._colour = texel + ((1.0f - texel.a) * _tint);
    result._colour = texel;

    return result;
}