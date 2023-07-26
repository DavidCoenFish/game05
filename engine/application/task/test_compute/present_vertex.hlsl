#include "present_interpolant.hlsli"

struct Vertex
{
    float2 _position : Position;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;
    result._position = float4(in_input._position.x, in_input._position.y, 0.0f, 1.0f);
    result._uv = float2(
        (in_input._position.x * 0.5) + 0.5, 
        (in_input._position.y * 0.5) + 0.5
        );
    return result;
}