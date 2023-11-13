#include "text_interpolant.hlsli"

struct Pixel
{
    float4 _colour : SV_TARGET0;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler_state : register(s0);

Pixel main(Interpolant in_input)
{
    Pixel result;

    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);
    float alpha = dot(texel, in_input._mask);
    // Premultiplied alpha blending
    result._colour = float4(
        in_input._colour.x * alpha,
        in_input._colour.y * alpha,
        in_input._colour.z * alpha,
        in_input._colour.w * alpha
        );

    return result;
}