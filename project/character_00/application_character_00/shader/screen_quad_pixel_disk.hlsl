#include "screen_quad_interpolant.hlsli"
#include "polar.hlsli" // b0
#include "ray_ray.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBufferDisk : register(b1)
{
	float4 _disc_xy_r;
	float4 _colour;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
    float3 camera_pos = GetCameraPos();

	float t = camera_pos.y / world_eye_ray.y;

	// 0.0 < t
    if (t < 0.0)
    {
        discard;
    }

	float ix = camera_pos.x + (world_eye_ray.x * t) - _disc_xy_r.x;
	float iz = camera_pos.z + (world_eye_ray.z * t) - _disc_xy_r.y;
	float temp = (ix * ix) + (iz * iz);

	float coverage = step(_disc_xy_r.z * _disc_xy_r.z, temp);

    result._color = float4(_colour.x, _colour.y, _colour.z, _colour.w * coverage);

    return result;
}