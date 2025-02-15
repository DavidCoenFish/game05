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
	float4 _colour_fog;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
    float world_up = world_eye_ray.y;

	float3 sky_color = lerp(_sky_colour_horizon.xyz, _sky_colour_pole.xyz, world_up);
	float3 ground_color = lerp(_ground_colour_pole.xyz, _ground_colour_horizon.xyz, 1.0 + world_up);

    float3 background = lerp(ground_color, sky_color, step(0.0, world_up));

	float temp = saturate(1.0 - abs(world_up));
	temp *= temp;
	temp *= temp;
	float fog_amount = saturate((temp * 6.0) - 5.0);
	float3 rgb = lerp(background, _colour_fog.xyz, fog_amount);

    result._color = float4(rgb.x, rgb.y, rgb.z, 1.0);

    return result;
}

