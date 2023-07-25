#include "multi_line_interpolant.hlsli"
#include "polar.hlsli" //b0

Texture2D _texture0 : register(t0);
Texture2D _texture1 : register(t1);
Texture2D _texture2 : register(t2);

struct Vertex
{
    float2 _position : POSITION0;
    int2 _uv : TEXCOORD0;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;

    float4 _texel0 = _texture0.Load(int3(in_input._uv.xy, 0));
    float4 _texel1= _texture1.Load(int3(in_input._uv.xy, 0));
    float4 _texel2 = _texture2.Load(int3(in_input._uv.xy, 0));

    float thickness = _texel0.w;
    //float unit_pixel_size = GetCameraUnitPixelSize();
    float radian_per_pixel = GetCameraRadianPerPixel();
    float radian_padding = thickness * radian_per_pixel;
    float2 radian_padding2 = float2(radian_padding, radian_padding);

    float2 uv_a = MakePolarUV(_texel0.xyz);
    float2 uv_mid0 = MakePolarUV(_texel0.xyz + (_texel1.xyz * (_texel1.w * 0.5)));
    float2 uv_mid1 = MakePolarUV(_texel0.xyz + (_texel1.xyz * (_texel1.w * 0.25)));
    float2 uv_mid2 = MakePolarUV(_texel0.xyz + (_texel1.xyz * (_texel1.w * 0.75)));
    float2 uv_b = MakePolarUV(_texel0.xyz + (_texel1.xyz * _texel1.w));
    float2 uv_low = min(uv_a - radian_padding2, uv_b - radian_padding2);
    uv_low = min(uv_low, uv_mid0 - radian_padding2);
    uv_low = min(uv_low, uv_mid1 - radian_padding2);
    uv_low = min(uv_low, uv_mid2 - radian_padding2);
    float2 uv_high = max(uv_a + radian_padding2, uv_b + radian_padding2);
    uv_high = max(uv_high, uv_mid0 + radian_padding2);
    uv_high = max(uv_high, uv_mid1 + radian_padding2);
    uv_high = max(uv_high, uv_mid2 + radian_padding2);

    float2 uv_delta = uv_high - uv_low;

    result._position = float4(
        uv_low.x + (uv_delta.x * step(0.0, in_input._position.x)),
        uv_low.y + (uv_delta.y * step(0.0, in_input._position.y)),
        0.0,
        1.0
        );

    result._uv = result._position.xy;
    result._line_pos_thickness = _texel0;
    result._line_dir_length = _texel1;
    result._line_colour = _texel2;

    return result;
}