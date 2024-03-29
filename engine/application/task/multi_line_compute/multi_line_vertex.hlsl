#include "multi_line_interpolant.hlsli"

Texture2D _texture0 : register(t0);
Texture2D _texture1 : register(t1);
//Texture2D _texture2 : register(t2);
//Texture2D _texture3 : register(t3);

struct Vertex
{
    float2 _position : POSITION0;
    int2 _uv : TEXCOORD0;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;
/*
    float4 _texel0 = _texture0.Load(int3(in_input._uv.xy, 0));
    float4 _texel1= _texture1.Load(int3(in_input._uv.xy, 0));
    float4 _texel2 = _texture2.Load(int3(in_input._uv.xy, 0));
    float4 _texel3 = _texture3.Load(int3(in_input._uv.xy, 0));

    float2 uv_delta = _texel0.zw - _texel0.xy;

    result._position = float4(
        _texel0.x + (uv_delta.x * step(0.0, in_input._position.x)),
        _texel0.y + (uv_delta.y * step(0.0, in_input._position.y)),
        0.0,
        1.0
        );

    result._uv = result._position.xy;
    result._line_pos_thickness = _texel1;
    result._line_dir_length = _texel2;
    result._line_colour = _texel3;

    return result;
*/
    float4 _texel0 = _texture0.Load(int3(in_input._uv.xy, 0));
    float4 _texel1 = _texture1.Load(int3(in_input._uv.xy, 0));

    float2 uv_delta = _texel0.zw - _texel0.xy;
    result._position = float4(
        _texel0.x + (uv_delta.x * step(0.0, in_input._position.x)),
        _texel0.y + (uv_delta.y * step(0.0, in_input._position.y)),
        0.0,
        1.0
        );

    result._uv = result._position.xy;
    result._line_pos_thickness = float4(0.0, 0.0, 0.0, 0.0);
    result._line_dir_length = float4(0.0, 0.0, 0.0, 0.0);
    result._line_colour = _texel1;

    return result;
}