#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

Pixel main(Interpolant in_input)
{
    Pixel result;

    int2 temp = int2(
        in_input._uv.x * _width_height_iwidth_iheight.x, 
        in_input._uv.y * _width_height_iwidth_iheight.y
        );

    float colour = 0.25;
    //colour += step(((int)round(temp.x + temp.y)) % 2, 0) * 0.125;
    colour += step(((int)round((temp.x / 1) + (temp.y / 1))) % 2, 0) * (0.5 * 0.7);
    colour += step(((int)round((temp.x / 4) + (temp.y / 4))) % 2, 0) * (0.5 * 0.5);
    colour += step(((int)round((temp.x / 16) + (temp.y / 16))) % 2, 0) * (0.5 * 0.3);
    colour += step(((int)round((temp.x / 64) + (temp.y / 64))) % 2, 0) * (0.5 * 0.1);

    // grid lines on 64 pixel
    //colour *= (1 - step(temp.x % 64, 0));
    //colour *= (1 - step(temp.y % 64, 0));

    result._colour = float4(colour, colour, colour, 1.0);

    return result;
}