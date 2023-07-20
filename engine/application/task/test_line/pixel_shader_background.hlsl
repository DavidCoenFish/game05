#include "interpolant.hlsli"
#include "polar.hlsli" //b0

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBuffer1 : register(b1)
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

    float3 world_eye_ray = MakeWorldEyeRay(in_input._uv);
    float world_up = world_eye_ray.z;

    float3 sun_norm = MakeScreenEyeRay(_sun_azimuth_altitude_sun_range.xy);

    float angle_sun_view = acos(dot(world_eye_ray, sun_norm));

    float3 sky_dome_color = CalcSkyDomeColor(angle_sun_view) + ((1.0 - world_up) * 0.25);
    float fog = (1.0 - abs(world_up));
    fog = max(0.0, min(1.0, pow(fog, _sky_tint_sky_turbitity.w)));
    float3 ground_color = _ground_tint.xyz * ((-world_up * 0.25) + 0.75);
    float3 rgb = lerp(ground_color, sky_dome_color, step(0.0, world_up));
    rgb = lerp(rgb, _fog_tint.xyz, fog);

    //result._color = float4(rgb.x, rgb.y, rgb.z, 1.0);
    //result._color = float4(sky_dome_color.x, sky_dome_color.y, sky_dome_color.z, 1.0);
    //result._color = float4(_sky_tint_sky_turbitity.x, _sky_tint_sky_turbitity.y, _sky_tint_sky_turbitity.z, 1.0);
    //result._color = float4(world_eye_ray.x, world_up, world_eye_ray.z, 1.0);
    //result._color = float4(sun_norm.x, sun_norm.y, sun_norm.z, 1.0); //blue
    //result._color = float4(angle_sun_view, angle_sun_view, angle_sun_view, 1.0);
    //result._color = float4(sky_dome_color.x, sky_dome_color.y, sky_dome_color.z, 1.0);
    result._color = float4(rgb.x, rgb.y, rgb.z, 1.0);

    return result;
}