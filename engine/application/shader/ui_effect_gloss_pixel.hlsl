#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

Pixel main(Interpolant in_input)
{
    Pixel result;

    // Premultipiled alpha
    //result = foreground + (1 - foreground.alpha) * background
    //result._colour = texel + ((1.0 - texel.a) * (_tint * shadow_alpha));
    result._colour = float4(1.0, 0.0, 0.0, 1.0);

    return result;
}