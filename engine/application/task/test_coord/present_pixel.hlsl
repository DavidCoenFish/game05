#include "present_interpolant.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

Pixel main(Interpolant in_interpolant)
{
    float4 textel = g_texture0.Sample(g_sampler_state, in_interpolant._uv);

    Pixel result;
    result._color = textel;
    return result;
}