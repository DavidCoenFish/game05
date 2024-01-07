#include "screen_quad_interpolant.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

Pixel main( Interpolant in_input )
{
    Pixel result;
    result._color = g_texture0.Sample(g_sampler_state, in_input._uv);
    //result._color.y = 1.0f;
    //result._color.z = 1.0f;
    //result._color.a = 1.0f;
    return result;
}

