#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    Pixel result;

    float average = (texel.x + texel.y + texel.z) / 3.0;
    float ratio = _constant_buffer_1_data0.x;
    float4 colour = lerp(texel, float4(average, average, average, texel.a), ratio);
    result._colour = colour * _tint;

    return result;
}