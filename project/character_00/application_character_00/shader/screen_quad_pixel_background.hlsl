#include "screen_quad_interpolant.hlsli"
#include "polar_util.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBufferSky : register(b0)
{
	float4 _sky_colour_pole;
	float4 _sky_colour_horizon;
	float4 _ground_colour_pole;
	float4 _ground_colour_horizon;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
    float world_up = world_eye_ray.y;

	//float3(0.0, 0.0, 1.0), float3(0.8, 0.8, 1.0)
	float3 sky_color = lerp(_sky_colour_horizon.xyz, _sky_colour_pole.xyz, world_up);

	//float3(0.91,0.737,0.259), float3(0.565, 0.416,0.0)
	float3 ground_color = lerp(_ground_colour_pole.xyz, _ground_colour_horizon.xyz, 1.0 + world_up);

	float3(1.0 + world_up, 1.0 + world_up, 1.0 + world_up);

    float3 rgb = lerp(ground_color, sky_color, step(0.0, world_up));

    result._color = float4(rgb.x, rgb.y, rgb.z, 1.0);

    return result;
}

