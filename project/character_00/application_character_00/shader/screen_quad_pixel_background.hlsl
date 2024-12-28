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
    float4 _sun_azimuth_altitude_sun_range;
    float4 _sun_tint_sky_spread;
    float4 _sky_tint_sky_turbitity;
    float4 _ground_tint;
    float4 _fog_tint;
};

float3 CalcSkyDomeColor(float in_gamma){
    float ratio = (1.0 - smoothstep(_sun_azimuth_altitude_sun_range.z, _sun_azimuth_altitude_sun_range.w, in_gamma));
    float3 sky = _sky_tint_sky_turbitity.xyz * exp(_sun_tint_sky_spread.w * in_gamma);
    float3 sun = _sun_tint_sky_spread.xyz;
    float3 result = lerp(sky, sun, ratio);
    return result;
}

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;
    float world_up = world_eye_ray.y;

    //float3 sun_norm = PolarToVector(_sun_azimuth_altitude_sun_range.xy);
    //float angle_sun_view = acos(dot(world_eye_ray, sun_norm));
    //float3 sky_dome_color = CalcSkyDomeColor(angle_sun_view) + ((1.0 - world_up) * 0.25);
    //float fog = (1.0 - abs(world_up));
    //fog = max(0.0, min(1.0, pow(fog, _sky_tint_sky_turbitity.w)));
    //float3 ground_color = _ground_tint.xyz * ((-world_up * 0.25) + 0.75);
    //float3 rgb = lerp(ground_color, sky_dome_color, step(0.0, world_up));
    //rgb = lerp(rgb, _fog_tint.xyz, fog);
    //result._color = float4(rgb.x, rgb.y, rgb.z, 1.0);

	//float grad = world_up * world_up;
    //float3 sky_color = _sky_tint_sky_turbitity.xyz * ((-grad * 0.25) + 0.75);
    //float3 sky_color = float4(world_up, world_up, world_up, 1.0);
	
    //float3 ground_color = _ground_tint.xyz * ((-grad * 0.25) + 0.75);


	float3 sky_color = lerp(float3(0.0, 0.0, 1.0), float3(0.8, 0.8, 1.0), world_up);
	//float3 ground_color = float3(1.0 + world_up, 1.0 + world_up, 1.0 + world_up);
	float3 ground_color = lerp(float3(0.91,0.737,0.259), float3(0.565, 0.416,0.0), 1.0 + world_up);
	
	float3(1.0 + world_up, 1.0 + world_up, 1.0 + world_up);

    float3 rgb = lerp(ground_color, sky_color, step(0.0, world_up));

    result._color = float4(rgb.x, rgb.y, rgb.z, 1.0);

    return result;
}

