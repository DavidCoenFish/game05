#include "triangle_interpolant.hlsli"

struct Vertex
{
    float2 _position : Position;
    float4 _color : COLOR0;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;
    result._position = float4(in_input._position.x, in_input._position.y, 0.0f, 1.0f);
    result._color = in_input._color;
    return result;
}