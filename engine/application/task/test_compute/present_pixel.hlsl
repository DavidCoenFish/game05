#include "present_interpolant.hlsli"

struct Pixel
{
    float4 _colour : SV_TARGET0;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler_state : register(s0);

Pixel main(Interpolant in_input)
{
    Pixel result;
    result._colour = g_texture.Sample(g_sampler_state, in_input._uv);
    return result;
}