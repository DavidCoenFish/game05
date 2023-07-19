/*
vec3 calcSkyDomeColor(float gamma){
	float ratio = (1.0 - smoothstep(u_sunRange.x, u_sunRange.y, gamma));
	vec3 sky = u_skyTint * exp(u_skySpread * radians(gamma));
	vec3 result = mix(sky, u_sunTint, ratio);
	return result;
}

void main() {
	vec3 screenEyeRay = texture2D(u_sampler0, v_uv).xyz;
	vec3 viewNorm = makeViewNorm(screenEyeRay);

	float sunZ = sin(radians(u_sunAzimuthAltitude.y));
	float sunXY = cos(radians(u_sunAzimuthAltitude.y));
	vec3 sunNorm = vec3(cos(radians(u_sunAzimuthAltitude.x)) * sunXY, sin(radians(u_sunAzimuthAltitude.x)) * sunXY, sunZ);

	float angleSunViewDegrees = degrees(acos(dot(viewNorm, sunNorm)));

	vec3 skyDomeColor = calcSkyDomeColor(angleSunViewDegrees) + ((1.0 - viewNorm.z) * 0.25);
	float fog = (1.0 - abs(viewNorm.z));
	fog = max(0.0, min(1.0, pow(fog, u_skyTurbitity)));
	vec3 rgb = mix((u_groundTint - (viewNorm.z * 0.5)), skyDomeColor, step(0.0, viewNorm.z));
	rgb = mix(rgb, u_fogTint, fog);
	gl_FragColor = vec4(rgb, 1.0);
}

	const m_sunAzimuthAltitude = Vector2FactoryFloat32(-90.0, 45.0);
	const m_sunTint = Vector3FactoryFloat32(255.0/255.0, 245.0/255.0, 235.0/255.0);
	const m_sunRange = Vector2FactoryFloat32(1.0, 5.0); 
	const m_skyTint = Vector3FactoryFloat32(10.0/255.0, 10.0/255.0, 255.0/255.0);
	const m_groundTint = Vector3FactoryFloat32(32.0/255.0, 16.0/255.0, 2.0/255.0);
	const m_fogTint = Vector3FactoryFloat32(200.0/255.0, 200.0/255.0, 200.0/255.0);
	const m_skySpread = -0.5; //-0.9;
	const m_skyTurbitity = 10.0;


*/

#include "polar.hlsli"

struct Interpolant
{
    float4 _position : SV_Position;
    float2 _uv : TEXCOORD0;
};

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBuffer0 : register(b0)
{
    float4 _camera_pos_fov_horizontal;
    float4 _camera_at_fov_vertical;
    float4 _camera_up_camera_far;
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
    float3 result = lerp(sky, _sun_tint_sky_spread.xyz, ratio);
    return result;
}

Pixel main( Interpolant in_input )
{
    Pixel result;

    float2 screen_radian = float2((in_input._uv.x * 0.5) * _camera_pos_fov_horizontal[3], (in_input._uv.y * 0.5) * _camera_at_fov_vertical[3]);
    float3 screen_eye_ray = MakeScreenEyeRay(screen_radian);

    float3 camera_right = cross(_camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz);
    float3 world_eye_ray = MakeWorldEyeRay(screen_eye_ray, _camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz, camera_right);

    //result._color = float4(world_eye_ray.x, world_eye_ray.y, world_eye_ray.z, 1.0);

    float sun_z = sin(_sun_azimuth_altitude_sun_range.y);
    float sun_xy = cos(_sun_azimuth_altitude_sun_range.y);
    float3 sun_norm = float3(cos(_sun_azimuth_altitude_sun_range.x) * sun_xy, sin(_sun_azimuth_altitude_sun_range.x) * sun_xy, sun_z);

    float angle_sun_view = acos(dot(world_eye_ray, sun_norm));

    float3 sky_dome_color = CalcSkyDomeColor(angle_sun_view) + ((1.0 - world_eye_ray.z) * 0.25);
    float fog = (1.0 - abs(world_eye_ray.z));
    fog = max(0.0, min(1.0, pow(fog, _sky_tint_sky_turbitity.w)));
    float3 rgb = lerp((_ground_tint.xyz - (world_eye_ray.z * 0.5)), sky_dome_color, step(0.0, world_eye_ray.z));
    rgb = lerp(rgb, _fog_tint.xyz, fog);

    result._color = float4(rgb.x, rgb.y, rgb.z, 1.0);

    return result;
}