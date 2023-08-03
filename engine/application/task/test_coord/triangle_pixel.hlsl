#include "triangle_interpolant.hlsli"

struct Pixel
{
    float4 _color : SV_TARGET0;
};

Pixel main(Interpolant in_interpolant)
{
    Pixel result;
    result._color = in_interpolant._color;
    return result;
}