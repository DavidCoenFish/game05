#include "screen_quad_interpolant.hlsli"
#include "polar.hlsli" // b0
#include "ray_ray.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _color : SV_TARGET0;
};

float CalculatePixelCoverage(float in_camera_unit_pixel_size, float in_line_pixel_thickness, float in_ray_ray_distance, float in_camera_distance)
{
    float pixel_distance = (in_ray_ray_distance / in_camera_distance) / in_camera_unit_pixel_size;

    float low = max(-0.5, pixel_distance - in_line_pixel_thickness);
    float high = min(0.5, pixel_distance + in_line_pixel_thickness);

    float coverage = saturate(high - low);
    return coverage;
}

float CalculateCoverage(
    float3 in_line_pos,
    float3 in_line_at,
    float in_line_length,
    float in_line_thickness,
    float3 in_camera_pos,
    float3 in_camera_at,
    float in_camera_far,
    float in_camera_unit_pixel_size
    )
{
    float coverage = 0.0;
    float3 pass_t1_t2 = RayRayClosest(
        in_line_pos, 
        in_line_at, 
        in_camera_pos, 
        in_camera_at
        );
    if (pass_t1_t2.x != 0.0)
    {
        float2 pass_distance = CalculateDistance(
            pass_t1_t2.y, 
            pass_t1_t2.z, 
            in_line_pos, 
            in_line_at, 
            in_line_length, 
            in_camera_pos, 
            in_camera_at,
            in_camera_far
            );
        if (pass_distance.x != 0.0)
        {
            coverage = CalculatePixelCoverage(
                in_camera_unit_pixel_size,
                in_line_thickness,
                pass_distance.y,
                pass_t1_t2.z
            );
        }
    }
    return coverage;
}

Pixel main( Interpolant in_input )
{
    Pixel result;

    //float3 world_eye_ray = MakeWorldEyeRay(in_input._uv);
    float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;

    float3 camera_pos = GetCameraPos();
    float camera_unit_pixel_size = GetCameraUnitPixelSize();
    float camera_far = GetCameraFar();
    float coverage = 0.0;

    // Get the intersection of world_eye_ray and the grid plane n[0,1,0], p0[0,0,0]
    float denom = world_eye_ray.y;
    if (abs(denom) < 0.0001)
    {
        discard;
    }

    float t = -camera_pos.y / denom;
    float3 grid_intersection = camera_pos + (world_eye_ray * t);
    float3 rounded_intersection = round(grid_intersection);

    coverage += CalculateCoverage(
        float3(rounded_intersection.x - 1.0, 0.0, rounded_intersection.z),
        float3(1.0, 0.0, 0.0),
        3.0,
        0.5,
        camera_pos,
        world_eye_ray,
        camera_far,
        camera_unit_pixel_size
        );

    coverage += CalculateCoverage(
        float3(rounded_intersection.x, 0.0, rounded_intersection.z - 1.0),
        float3(0.0, 0.0, 1.0),
        3.0,
        0.5,
        camera_pos,
        world_eye_ray,
        camera_far,
        camera_unit_pixel_size
        );

    float distance = length(grid_intersection - camera_pos);
    float fade = saturate(1.0 - (distance / 100.0));

    coverage = min(1.0, coverage * fade * 0.25);

    //float color = (1.0 - step(min(abs(rounded_intersection.x), abs(rounded_intersection.z)), 0.0)) * 0.125;
    //float color = (1.0 - step(abs(rounded_intersection.x) + abs(rounded_intersection.z), 0.0)) * 0.125;
    float color = 0.0;

    result._color = float4(color, color, color, coverage);

    return result;
}