#include "screen_quad_interpolant.hlsli"
#include "polar.hlsli" // b0
#include "ray_ray.hlsli"

Texture2D g_texture0 : register(t0); // world camera ray, b0 has _camera_pos
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBufferSphere : register(b1)
{
	float4 _ray_origin_thickness;
	float4 _ray_normal_length;
	float4 _ray_color;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

	float3 camera_pos = GetCameraPos();
    float3 camera_norm = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;

	//float3 intersect = IntersectRayRay(camera_pos, camera_norm, _ray_origin_thickness.xyz, _ray_normal_length.xyz);
	float3 intersect = RayRayClosest(camera_pos, camera_norm, _ray_origin_thickness.xyz, _ray_normal_length.xyz);

	if ((intersect.x == 0.0) 
		|| (intersect.y < 0.0) 
		//|| (intersect.z < 0.0) 
		//|| (_ray_normal_length.w < intersect.z)
		)
	{
		discard;
	}

	float t = saturate(intersect.z / _ray_normal_length.w) * _ray_normal_length.w;

	float3 camera_ray_nearest_point = camera_pos + (camera_norm * intersect.y);
	float3 line_ray_nearest_point = _ray_origin_thickness.xyz + (_ray_normal_length.xyz * t);

	float world_dist = distance(camera_ray_nearest_point, line_ray_nearest_point);

    float pixel_distance = world_dist / GetCameraUnitPixelSize();

    float coverage = saturate(_ray_origin_thickness.w - pixel_distance);

    result._color = _ray_color * coverage;

    return result;
}