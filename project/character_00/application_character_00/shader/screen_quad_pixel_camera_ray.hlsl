#include "polar.hlsli"
#include "screen_quad_interpolant.hlsli"

struct Pixel
{
    float4 _color : SV_TARGET0;
};

Pixel main( Interpolant in_input )
{
    Pixel result;

    float3 world_eye_ray = MakeWorldEyeRay(float2(
        (in_input._uv.x * 2.0) - 1.0,
        ((1.0 - in_input._uv.y) * 2.0) - 1.0
        ));

    result._color = float4(world_eye_ray, 1.0);
    //result._color = float4(in_input._uv.x, 1.0 - in_input._uv.y, 0.0, 1.0);

    return result;
}