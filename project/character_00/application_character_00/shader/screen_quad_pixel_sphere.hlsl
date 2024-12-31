#include "screen_quad_interpolant.hlsli"
#include "polar.hlsli" // b0
#include "ray_ray.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBufferSphere : register(b0)
{
	float4 _pos_radius;
	float4 _colour;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
	float3 camera_pos = GetCameraPos();

	float coverage = 0.0;

    result._color = float4(_colour.x, _colour.y, _colour.z, _colour.w * coverage);

    return result;
}