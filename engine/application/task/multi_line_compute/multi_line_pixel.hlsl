#include "multi_line_interpolant.hlsli"
#include "../../shader/polar.hlsli" //b0
#include "../../shader/ray_ray.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler_state : register(s0);

struct Pixel
{
    float4 _colour : SV_TARGET0;
};

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

    ////float3 world_eye_ray = MakeWorldEyeRay(in_input._uv);
    //float3 world_eye_ray = g_texture0.Sample(g_sampler_state, in_input._uv).xyz;

    //float3 camera_pos = GetCameraPos();
    //float3 camera_at = GetCameraAt();
    //float camera_unit_pixel_size = GetCameraUnitPixelSize();
    //float camera_far = GetCameraFar();
    //float coverage = 0.0;

    //float3 pass_t1_t2 = RayRayClosest(
    //    in_input._line_pos_thickness.xyz,
    //    in_input._line_dir_length.xyz,
    //    camera_pos,
    //    world_eye_ray
    //    );
    //if (pass_t1_t2.x != 0.0)
    //{
    //    float2 pass_distance = CalculateDistance(
    //        pass_t1_t2.y,
    //        pass_t1_t2.z,
    //        in_input._line_pos_thickness.xyz,
    //        in_input._line_dir_length.xyz,
    //        in_input._line_dir_length.w,
    //        camera_pos,
    //        world_eye_ray,
    //        camera_far
    //        );
    //    if (pass_distance.x != 0.0)
    //    {
    //        coverage = CalculatePixelCoverage(
    //            camera_unit_pixel_size,
    //            in_input._line_pos_thickness.w,
    //            pass_distance.y,
    //            pass_t1_t2.z
    //        );
    //    }
    //}

    //float a = coverage * in_input._line_colour.a;
    //result._colour = float4(in_input._line_colour.xyz * a, a);
    //result._colour = float4(1.0, 0.0, 0.0, 1.0);
    result._colour = in_input._line_colour;
    //result._colour = float4(a, a, a, 1.0);
    //result._colour = float4(in_input._line_dir_length.xyz, 1.0);
    //result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);
    //result._colour = float4(coverage, coverage, coverage, 1.0);

    return result;
}

