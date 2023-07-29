#include "text_interpolant.hlsli"

struct Vertex
{
    // [-1 ... 1]
    float2 _position : Position;
    float2 _uv : TEXCOORD0;

    // Bandwidth vrs book-keeping to convert choice of rgba channel for data as foat4 or int
    float4 _mask : COLOR0;
    //float3 _tint; ?
    //int _mask; ?

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
    return result;
}
