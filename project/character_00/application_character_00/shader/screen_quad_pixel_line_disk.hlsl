#include "screen_quad_interpolant.hlsli"
#include "polar.hlsli" // b0
#include "ray_ray.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBufferCircle : register(b1)
{
	float4 _circle_pos_radius;
	float4 _circle_normal_thickness;
	float4 _color;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
    float3 camera_pos = GetCameraPos() - _circle_pos_radius.xyz;

	//https://stackoverflow.com/questions/23975555/how-to-calculate-a-ray-plane-intersection
	float denom = dot(world_eye_ray, _circle_normal_thickness.xyz);
	float t = 0.0;
	if (0.0001 < abs(denom))
	{
		t = -1.0 * dot(_circle_normal_thickness.xyz, camera_pos) / denom;
	}

	if (0.0f < t)
	{
		float3 intersection_on_cirlce_plane = camera_pos + (world_eye_ray * t);
		float world_dist = abs(length(intersection_on_cirlce_plane) - _circle_pos_radius.w);
		float pixel_distance = world_dist / GetCameraUnitPixelSize();

		float coverage = saturate(_circle_normal_thickness.w - pixel_distance);

		result._color = _color * coverage;
	}
	else
	{
		discard;
	}

    return result;
}