#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    Pixel result;

    float alpha = texel.a * _tint.a;
    //result = foreground + (1 - foreground.alpha) * background
    result._colour = texel + ((1.0f - texel.a) * _tint);

    return result;
}