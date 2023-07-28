#include "../../shader/screen_quad_interpolant.hlsli"

cbuffer ConstantBuffer0 : register(b0)
{
    int2 _screen_width_height;
};

struct Pixel
{
    float4 _colour : SV_TARGET0;
};

Pixel main(Interpolant in_input)
{
    Pixel result;

    //int x = (int)round(in_input._uv.x * _screen_width_height.x);
    //int y = (int)round(in_input._uv.y * _screen_width_height.y);
    //int value = (x + y) % 2;
    //float colour = 1.0 - (((float)step(value, 0)) * 0.5);

    //result._colour = float4(colour, colour, colour, 1.0);
    ////result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);
    //result._colour = float4(
    //    _screen_width_height.x / 784.0,
    //    _screen_width_height.y / 561.0,
    //    1.0, 
    //    1.0
    //    );

    float temp = (in_input._uv.x * _screen_width_height.x) + (in_input._uv.y * _screen_width_height.y);
    int value = ((int)round(temp)) % 2;
    float colour = 1.0 - (((float)step(value, 0)) * 0.5);
    result._colour = float4(colour, colour, colour, 1.0);

    return result;
}