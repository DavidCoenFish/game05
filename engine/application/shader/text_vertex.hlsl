#include "text_interpolant.hlsli"

struct Vertex
{
    // [-1 ... 1]
    float2 _position : Position;
    float2 _uv : TEXCOORD0;

    // Bandwidth vrs book-keeping to convert choice of rgba channel for data as foat4 or int
    // Mask also allows us to have icons in the font texture page if we want to be clever
    float4 _mask : COLOR0;

    float4 _colour : COLOR1;

};

Interpolant main(Vertex in_input)
{
    Interpolant result;
    result._position = float4(
        in_input._position.x,
        in_input._position.y,
        0.0f, 
        1.0f
        );
    result._uv = in_input._uv;
    result._mask = in_input._mask;
    result._colour = in_input._colour;
    return result;
}
