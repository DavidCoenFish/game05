#include "screen_quad_interpolant.hlsli"
#include "polar.hlsli" // b0
#include "ray_ray.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBufferSphere : register(b1)
{
	float4 _sphere_pos_radius;
	float4 _sphere_color;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
	float3 camera_pos = GetCameraPos();

	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
	float3 L = float3(_sphere_pos_radius.x - camera_pos.x, _sphere_pos_radius.y - camera_pos.y, _sphere_pos_radius.z - camera_pos.z);
	float tca = dot(L, world_eye_ray);
	if (tca < 0.0)
	{
		discard;
	}

    float d2 = dot(L, L) - (tca * tca);
	float radius = _sphere_pos_radius.w;
    if (radius * radius < d2)
	{
		discard;
	}

    result._color = _sphere_color;

    return result;
}