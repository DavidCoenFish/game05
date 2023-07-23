#include "interpolant.hlsli"
#include "polar.hlsli" //b0

struct Pixel
{
    float4 _color : SV_TARGET0;
};

// https://en.wikipedia.org/wiki/Skew_lines#Distance%5B/url%5D
float3 RayRayClosest(float3 in_p1, float3 in_d1, float3 in_p2, float3 in_d2)
{
    float3 n = cross(in_d1, in_d2);
    float3 n1 = float3(0.0, 0.0, 0.0);
    float3 n2 = float3(0.0, 0.0, 0.0);
    float t1 = 0.0f;
    float t2 = 0.0f;
    float3 result = float3(0.0, 0.0, 0.0);

    // rays are not that close to parrallel
    if (0.0001 < dot(n, n))
    {
        n1 = cross(in_d1, n);
        n2 = cross(in_d2, n);
        t1 = dot(in_p2 - in_p1, n2) / dot(in_d1, n2);
        t2 = dot(in_p1 - in_p2, n1) / dot(in_d2, n1);
        result = float3(1.0, t1, t2);
    }

    return result;
}

float2 CalculateDistance(float in_t1, float in_t2, float3 in_p1, float3 in_d1, float in_l1, float3 in_p2, float3 in_d2, float in_l2)
{
    float3 c1 = float3(0.0, 0.0, 0.0);
    float3 c2 = float3(0.0, 0.0, 0.0);
    float d = 0.0;
    float2 result = float2(0.0, 0.0);

    if ((in_t1 < 0.0) || 
        (in_l1 < in_t1) ||
        (in_t2 < 0.0) || 
        (in_l2 < in_t2))
    {
        // Nop
    }
    else
    {
        c1 = in_p1 + (in_d1 * in_t1);
        c2 = in_p2 + (in_d2 * in_t2);
        d = length(c1 - c2);
        result = float2(1.0, d);
    }

    return result;
}

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

    float3 world_eye_ray = MakeWorldEyeRay(in_input._uv);

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