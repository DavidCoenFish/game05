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
	float4 _color;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
    float3 camera_pos = GetCameraPos();

	float t = camera_pos.y / world_eye_ray.y;

	if (0.0 < t)
    {
        discard;
    }

	float ix = camera_pos.x - (world_eye_ray.x * t) - _disc_xy_r.x;
	float iz = camera_pos.z - (world_eye_ray.z * t) - _disc_xy_r.y;
	float temp = (ix * ix) + (iz * iz);

	float coverage = step(temp, _disc_xy_r.z * _disc_xy_r.z);

    result._color = float4(_color.x, _color.y, _color.z, _color.w * coverage);
    result._color = float4(coverage, coverage, coverage, 1.0);
    result._color = _color * coverage;

    return result;
}