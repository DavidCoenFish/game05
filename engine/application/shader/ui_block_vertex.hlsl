#include "ui_block_interpolant.hlsli"

struct Vertex
{
    // [-1 ... 1] (bottom left, top right)
    float2 _position : Position;

    // [0 ... 1] (top left, bottom right)
    float2 _uv : TEXCOORD0;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;

    result._position = float4(
        in_input._position.x,
        in_input._position.y,
        0.0,
        1.0
        );

    result._uv = in_input._uv;

    return result;
}
