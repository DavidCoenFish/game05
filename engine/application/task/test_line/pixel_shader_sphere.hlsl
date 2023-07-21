#include "interpolant.hlsli"
#include "polar.hlsli" //b0

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBuffer1 : register(b1)
{
    float4 _sphere_pos_radius;
    float4 _sphere_tint_edge_thickness_pixels;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = MakeWorldEyeRay(in_input._uv);
    float3 camera_pos = GetCameraPos();
    float camera_unit_pixel_size = GetCameraUnitPixelSize();

    //float3 camera_to_sphere = _sphere_pos_radius.xyz
    float distance_along_eye_ray = dot(world_eye_ray, _sphere_pos_radius.xyz - camera_pos);
    if (distance_along_eye_ray < 0.0)
    {
        // behind camera
        discard;
    }

    float eye_to_sphere_center = length(_sphere_pos_radius.xyz - (camera_pos + (world_eye_ray * distance_along_eye_ray)));
    float pixel_distance = (eye_to_sphere_center - _sphere_pos_radius .w) / distance_along_eye_ray / camera_unit_pixel_size;

    float low = max(-0.5, pixel_distance - _sphere_tint_edge_thickness_pixels.w);
    float high = min(0.5, pixel_distance + _sphere_tint_edge_thickness_pixels.w);

    float coverage = max(0.0, min(1.0, (high - low)));

    result._color = float4(
        _sphere_tint_edge_thickness_pixels.x, 
        _sphere_tint_edge_thickness_pixels.y,
        _sphere_tint_edge_thickness_pixels.z,
        coverage
        );
    //result._color = float4(coverage, coverage, coverage, 1.0);

    return result;
}

