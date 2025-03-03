#include "screen_quad_interpolant.hlsli"
#include "polar.hlsli" // b0
#include "ray_ray.hlsli"

#define HALFPI 1.5707963267948966192313216916398

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
	float4 _color : SV_TARGET0;
};

cbuffer ConstantBufferSpline : register(b1)
{
	float4 _p0_thickness;
	float4 _p1;
	float4 _p2;
	float4 _color;
};

// https://www.shadertoy.com/view/3sjXDm
//Find roots using Cardano's method. http://en.wikipedia.org/wiki/Cubic_function#Cardano.27s_method
float2 SolveCubic2(float a, float b, float c)
{
	float p = b - a * a / 3.0; 
	float p3 = p * p * p;
	float q = a * (2.0 * a * a - 9.0 * b) / 27.0 + c;
	float d = q * q + 4.0 * p3 / 27.0;
	float offset = -a / 3.0;
	if (d > 0.0)
	{
		float z = sqrt(d);
		float2 x = (float2(z, -z) - q) * 0.5;
		float2 uv = sign(x) * pow(abs(x), float2(1.0 / 3.0, 1.0 / 3.0));
		float temp = offset + uv.x + uv.y;
		return float2(temp, temp);
	}
	float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
	float m = cos(v);
	float n = sin(v) * 1.732050808;
	return float2(m + m, -n - m) * sqrt(-p / 3.0) + offset;
}

// How to resolve the equation below can be seen on this image.
// http://www.perbloksgaard.dk/research/DistanceToQuadraticBezier.jpg
float3 IntersectQuadraticBezier(float3 p0, float3 p1, float3 p2, float thickness)
{
	float2 A2 = p1.xy - p0.xy;
	float2 B2 = p2.xy - p1.xy - A2;
	float3 r = float3(-3. * dot(A2, B2), dot(-p0.xy, B2) - 2. * dot(A2, A2), dot(-p0.xy, A2)) / -dot(B2, B2);
	float2 t = clamp(SolveCubic2(r.x, r.y, r.z), 0., 1.);
	float3 A3 = p1 - p0;
	float3 B3 = p2 - p1 - A3;
	float3 D3 = A3 * 2.;
	float3 pos1 = (D3 + B3 * t.x) * t.x + p0;
	float3 pos2 = (D3 + B3 * t.y) * t.y + p0;
	pos1.xy /= thickness;
	pos2.xy /= thickness;
	float pos1Len = length(pos1.xy);
	if (pos1Len > 1.)
	{
		pos1 = float3(1e8, 1e8, 1e8);
	}
	float pos2Len = length(pos2.xy);
	if (pos2Len > 1.)
	{
		pos2 = float3(1e8, 1e8, 1e8);
	}
	pos1.z -= cos(pos1Len * HALFPI) * thickness;
	pos2.z -= cos(pos2Len * HALFPI) * thickness;
	return (length(pos1) < length(pos2)) ? float3(pos1Len, pos1.z, t.x) : float3(pos2Len, pos2.z, t.y);
}

float3x3 InverseView(float2 a)
{
	float2 c = cos(a);
	float2 s = sin(a);
	return float3x3(c.y, 0., -s.y, s.x * s.y, c.x, s.x * c.y, c.x * s.y, -s.x, c.x * c.y);
}

Pixel main( Interpolant in_input )
{
	float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
	float3 world_eye_pos = GetCameraPos();

	float3x3 m = InverseView(float2(asin(-world_eye_ray.y), atan2(world_eye_ray.x, world_eye_ray.z)));
	float3 lRes = IntersectQuadraticBezier(
		mul((_p0_thickness.xyz - world_eye_pos),m), 
		mul((_p1.xyz - world_eye_pos),m), 
		mul((_p2.xyz - world_eye_pos),m), 
		_p0_thickness.w);

	Pixel result;
	result._color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	result._color = float4(lRes.x, lRes.y, lRes.z, 1.0f);

	return result;
}