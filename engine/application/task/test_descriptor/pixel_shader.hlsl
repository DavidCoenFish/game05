#include "../../shader/screen_quad_interpolant.hlsli"

Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);
Texture2D g_texture2 : register(t2);
Texture2D g_texture3 : register(t3);
Texture2D g_texture4 : register(t4);
Texture2D g_texture5 : register(t5);
Texture2D g_texture6 : register(t6);
Texture2D g_texture7 : register(t7);
Texture2D g_texture8 : register(t8);

SamplerState g_sampler_state0 : register(s0);
SamplerState g_sampler_state1 : register(s1);
SamplerState g_sampler_state2 : register(s2);
SamplerState g_sampler_state3 : register(s3);
SamplerState g_sampler_state4 : register(s4);
SamplerState g_sampler_state5 : register(s5);
SamplerState g_sampler_state6 : register(s6);
SamplerState g_sampler_state7 : register(s7);
SamplerState g_sampler_state8 : register(s8);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

Pixel main( Interpolant in_input )
{
    Pixel result;
    float4 colour_0 = g_texture0.Sample(g_sampler_state0, in_input._uv);
    float4 colour_1 = g_texture1.Sample(g_sampler_state1, in_input._uv);
    float4 colour_2 = g_texture2.Sample(g_sampler_state2, in_input._uv);
    float4 colour_3 = g_texture3.Sample(g_sampler_state3, in_input._uv);
    float4 colour_4 = g_texture4.Sample(g_sampler_state4, in_input._uv);
    float4 colour_5 = g_texture5.Sample(g_sampler_state5, in_input._uv);
    float4 colour_6 = g_texture6.Sample(g_sampler_state6, in_input._uv);
    float4 colour_7 = g_texture7.Sample(g_sampler_state7, in_input._uv);
    float4 colour_8 = g_texture8.Sample(g_sampler_state8, in_input._uv);
    
    //result._color = (colour_0 + colour_1 + colour_2 + colour_3 + colour_4) * 0.2;
    //result._color = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);
    //result._color = float4(colour_0.w, 0.0, 0.0, 1.0);
    //result._color = float4(colour_0.w, colour_1.w, 0.0, 1.0);
    //result._color = float4(colour_8.w, colour_8.w, colour_8.w, 1.0);
    //float r = (colour_0.w + colour_1.w + colour_2.w) * 0.33;
    //float g = (colour_3.w + colour_4.w + colour_5.w) * 0.33;
    //float b = ((colour_6.w + colour_7.w + colour_8.w) * 0.5);
    //result._color = float4(r, g, b, 1.0);

    float c = (colour_0.w + colour_1.w + colour_2.w + colour_3.w + colour_4.w + colour_5.w + colour_6.w + colour_7.w + colour_8.w) * 0.5;

    result._color = float4(c, c, c, 1.0);

    //result._color = colour_0;

    return result;
}

