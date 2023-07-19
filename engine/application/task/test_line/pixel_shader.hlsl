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
    float4 _line_pos_radian_per_pixel;
    float4 _line_at_length;
    float _line_thickness;
};

// https://en.wikipedia.org/wiki/Skew_lines#Distance%5B/url%5D
float3 RayRayClosest(float3 in_p1, float3 in_d1, float3 in_p2, float3 in_d2)
{
    float3 n = cross(in_d1, in_d2);
    float3 n1 = float3(0.0, 0.0, 0.0);
    float3 n2 = float3(0.0, 0.0, 0.0);
    float t1 = 0.0f;
    float t2 = 0.0f;

    if (dot(n, n) < 0.00001)
    {
        // rays are parrallel
        return float3(0.0, 0.0, 0.0);
    }

    n1 = cross(in_d1, n);
    n2 = cross(in_d2, n);
    t1 = dot(in_p2 - in_p1, n2) / dot(in_d1, n2);
    t2 = dot(in_p1 - in_p2, n1) / dot(in_d2, n1);

    return float3(1.0, t1, t2);
}

float2 CalculateDistance(float in_t1, float in_t2, float3 in_p1, float3 in_d1, float in_l1, float3 in_p2, float3 in_d2, float in_l2)
{
    float3 c1 = float3(0.0, 0.0, 0.0);
    float3 c2 = float3(0.0, 0.0, 0.0);
    float d = 0.0;

    if ((in_t1 < 0.0) || (in_l1 < in_t1))
    {
        return float2(0.0, 0.0);
    }
    if ((in_t2 < 0.0) || (in_l2 < in_t2))
    {
        return float2(0.0, 0.0);
    }
    c1 = in_p1 + (in_d1 * in_t1);
    c2 = in_p2 + (in_d2 * in_t2);
    d = length(c1 - c2);
    return float2(1.0, d);
}

float CalculateCoverage(float in_radian_per_pixel, float in_line_pixel_thickness, float in_ray_ray_distance, float in_camera_distance)
{
#if 0
    if (in_ray_ray_distance < 0.01)
    {
        return 1.0;
    }
    return 0.0;

#else
    float angle = atan2(in_ray_ray_distance, in_camera_distance);
    float pixel_distance = angle / in_radian_per_pixel;

    float low = max(-0.5, pixel_distance - (0.5 * in_line_pixel_thickness));
    float high = min(0.5, pixel_distance + (0.5 * in_line_pixel_thickness));

    float coverage = high - low;
    return coverage;

#endif
}

Pixel main( Interpolant in_input )
{
    Pixel result;

    float2 screen_radian = float2((in_input._uv.x * 0.5) * _camera_pos_fov_horizontal[3], (in_input._uv.y * 0.5) * _camera_at_fov_vertical[3]);
    float3 screen_eye_ray = MakeScreenEyeRay(screen_radian);

    float3 camera_right = cross(_camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz);
    float3 world_eye_ray = MakeWorldEyeRay(screen_eye_ray, _camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz, camera_right);

    float3 pass_t1_t2 = RayRayClosest(_line_pos_radian_per_pixel.xyz, _line_at_length.xyz, _camera_pos_fov_horizontal.xyz, world_eye_ray);
    if (pass_t1_t2.x == 0.0)
    {
        discard;
    }

    float2 pass_distance = CalculateDistance(pass_t1_t2.y, pass_t1_t2.z, _line_pos_radian_per_pixel.xyz, _line_at_length.xyz, _line_at_length.w, _camera_pos_fov_horizontal.xyz, world_eye_ray, _camera_up_camera_far.w);
    if (pass_distance.x == 0.0)
    {
        discard;
    }

    float coverage = CalculateCoverage(_line_pos_radian_per_pixel.w, _line_thickness, pass_distance.y, pass_t1_t2.z);

    result._color = float4(coverage, coverage, coverage, 1.0);
    //result._color = float4(_camera_pos_fov_horizontal.x, _camera_pos_fov_horizontal.y, _camera_pos_fov_horizontal.z, 1.0f);
    //result._color = float4(_line_pos_radian_per_pixel.x, _line_pos_radian_per_pixel.y, _line_pos_radian_per_pixel.z, 1.0);
    //result._color = float4(0.0, 0.0, 0.0, coverage);

    return result;
}