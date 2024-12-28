#include "screen_quad_interpolant.hlsli"

struct Vertex
{
    // [0 .. 1]
    float2 _position : Position;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;
    result._position = float4(
        (in_input._position.x * 2.0) - 1.0,
        (in_input._position.y * 2.0) - 1.0,
        0.0f, 
        1.0f
        );
    result._uv = float2(in_input._position.x, 1.0 - in_input._position.y);
    return result;
}
