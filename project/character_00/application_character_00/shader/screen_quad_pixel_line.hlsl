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

/*
	https://stackoverflow.com/questions/2316490/the-algorithm-to-find-the-point-of-intersection-of-two-3d-line-segment/10288710#10288710

	result[0] != 0.0 if result valid
	result[1] distance along ray A of the nearest point between rays
	result[2] distance along ray B of the nearest point between rays
*/
float3 IntersectRayRay(float3 in_a_pos, float3 in_a_norm, float3 in_b_pos, float3 in_b_norm)
{
	float3 result = float3(0.0, 0.0, 0.0);
	float3 c = float3(in_b_pos.x - in_a_pos.x, in_b_pos.y - in_a_pos.y, in_b_pos.z - in_a_pos.z);
	//~a.~b	~b.~c	~a.~c	~a.~a	~b.~b
	float ab = dot(in_a_norm, in_b_norm);
	float bc = dot(in_b_norm, c);
	float ac = dot(in_a_norm, c);
	float aa = dot(in_a_norm, in_a_norm);
	float bb = dot(in_b_norm, in_b_norm);

	//D=A + ~a * ((-(~a.~b)(~b.~c) + (~a.~c)(~b.~b))/((~a.~a)(~b.~b)-(~a.~b)(~a.~b))
	//E=B + ~b((~a.~b)(~a.~c)-(~b.~c)(~a.~a))/((~a.~a)(~b.~b)-(~a.~b)(~a.~b))
	float denominator = (aa * bb) - (ab * ab); 
	if (0.0 == denominator)
	{
		return result;
	}

	result[0] = 1.0;
	result[1] = ((ac * bb) - (ab * bc)) / denominator;
	result[2] = ((ab * ac) - (bc * aa)) / denominator;

	return result;
}

Pixel main( Interpolant in_input )
{
    Pixel result;

	float3 camera_pos = GetCameraPos();
    float3 camera_norm = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;

	float3 intersect = IntersectRayRay(camera_pos, camera_norm, _ray_origin_thickness.xyz, _ray_normal_length.xyz);

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