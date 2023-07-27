#include "multi_line_interpolant.hlsli"
#include "../../shader/polar.hlsli" //b0

struct Pixel
{
    float4 _colour : SV_TARGET0;
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

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = MakeWorldEyeRay(in_input._uv);

    float3 camera_pos = GetCameraPos();
    float3 camera_at = GetCameraAt();
    float camera_unit_pixel_size = GetCameraUnitPixelSize();
    float camera_far = GetCameraFar();
    float coverage = 0.0;

    float3 pass_t1_t2 = RayRayClosest(
        in_input._line_pos_thickness.xyz,
        in_input._line_dir_length.xyz,
        camera_pos,
        world_eye_ray
        );
    if (pass_t1_t2.x != 0.0)
    {
        float2 pass_distance = CalculateDistance(
            pass_t1_t2.y,
            pass_t1_t2.z,
            in_input._line_pos_thickness.xyz,
            in_input._line_dir_length.xyz,
            in_input._line_dir_length.w,
            camera_pos,
            world_eye_ray,
            camera_far
            );
        if (pass_distance.x != 0.0)
        {
            coverage = CalculatePixelCoverage(
                camera_unit_pixel_size,
                in_input._line_pos_thickness.w,
                pass_distance.y,
                pass_t1_t2.z
            );
        }
    }

    float a = coverage * in_input._line_colour.a;
    result._colour = float4(in_input._line_colour.xyz * a, a);
    //result._colour = float4(1.0, 0.0, 0.0, 1.0);
    //result._colour = in_input._line_colour;
    //result._colour = float4(a, a, a, 1.0);
    //result._colour = float4(in_input._line_dir_length.xyz, 1.0);
    //result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);
    //result._colour = float4(coverage, coverage, coverage, 1.0);

    return result;
}

