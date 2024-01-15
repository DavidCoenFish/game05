#include "interpolant.hlsli"

struct Vertex
{
    float2 _position : POSITION;
    float2 _p0 : COLOR0;
    float2 _p1 : COLOR1;
    float2 _p2 : COLOR2;
    float2 _thickness : COLOR3;
    float2 _dim : COLOR4;
    float2 _uv : TEXCOORD0;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;
    result._position = float4(in_input._position.x, in_input._position.y, 0.0f, 1.0f);
    result._uv = in_input._uv;
    result._p0 = in_input._p0;
    result._p1 = in_input._p1;
    result._p2 = in_input._p2;
    result._thickness = in_input._thickness;
    result._dim = in_input._dim;
    return result;
}