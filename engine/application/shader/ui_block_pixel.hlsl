#include "ui_block_interpolant.hlsli"

struct Pixel
{
    float4 _colour : SV_TARGET0;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler_state : register(s0);

Pixel main(Interpolant in_input)
{
    Pixel result;
    float2 uv = in_input._uv;
    uv.y = 1.0 - uv.y;
    float4 texel = g_texture.Sample(g_sampler_state, uv);
    result._colour = texel;
    //result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);

    return result;
}