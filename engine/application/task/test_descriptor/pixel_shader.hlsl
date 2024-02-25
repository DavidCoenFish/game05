#include "../../shader/screen_quad_interpolant.hlsli"

Texture2D g_texture0 : register(t0);
//Texture2D g_texture1 : register(t1);
//Texture2D g_texture2 : register(t2);
//Texture2D g_texture3 : register(t3);
//Texture2D g_texture4 : register(t4);
SamplerState g_sampler_state : register(s0);
//SamplerState g_sampler_state0 : register(s0);
//SamplerState g_sampler_state1 : register(s1);
//SamplerState g_sampler_state2 : register(s2);
//SamplerState g_sampler_state3 : register(s3);
//SamplerState g_sampler_state4 : register(s4);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

Pixel main( Interpolant in_input )
{
    Pixel result;
    float4 colour_0 = g_texture0.Sample(g_sampler_state, in_input._uv);
    //float4 colour_1 = g_texture1.Sample(g_sampler_state, in_input._uv);
    //float4 colour_2 = g_texture2.Sample(g_sampler_state, in_input._uv);
    //float4 colour_3 = g_texture3.Sample(g_sampler_state, in_input._uv);
    //float4 colour_4 = g_texture4.Sample(g_sampler_state, in_input._uv);
    
    //result._color = (colour_0 + colour_1 + colour_2 + colour_3 + colour_4) * 0.2;
    //result._color = float4(1.0, 0.0, 0.0, 1.0);
    result._color = colour_0;

    return result;
}

