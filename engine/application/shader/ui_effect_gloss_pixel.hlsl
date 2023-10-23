#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

float SumQuadrantSample(
    float2 in_pivot,
    float2 in_offset,
    float2 in_iwidth_iheight
    )
{
    float2 uv = (in_pivot + in_offset) * in_iwidth_iheight;
    float ratio = step(0.0, uv.x) * step(0.0, uv.y) * step(uv.x, 1.0) * step(uv.y, 1.0);

    float texel = 1.0 - g_texture.Sample(g_sampler_state, uv).a;
    float result = (texel * ratio) + (1.0 - ratio);
    return result;
}

float4 SumQuadrant(float4 in_quadrant_sum, float2 in_pivot, float in_a, float in_b, float2 in_iwidth_iheight)
{
    float4 result = in_quadrant_sum + float4(
        SumQuadrantSample(in_pivot, float2(-in_a, -in_b), in_iwidth_iheight),
        SumQuadrantSample(in_pivot, float2(in_b, -in_a), in_iwidth_iheight),
        SumQuadrantSample(in_pivot, float2(in_a, in_b), in_iwidth_iheight),
        SumQuadrantSample(in_pivot, float2(-in_b, in_a), in_iwidth_iheight)
        );
    return result;
}

// return gloss amount and darken amount
float2 CalculateGlossDark(
    float2 in_uv, 
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    // ensure the uv is at pixel center, the extra +[0.05,0.05] inside the round is to avoid a horible (accuracy?) step
    float2 pivot = (floor((in_uv * in_width_height) + float2(0.05, 0.05))) + float2(0.5, 0.5);
    //float2 pivot = in_uv * in_width_height;

    float4 quadrant_sum = float4(0.0, 0.0, 0.0, 0.0);
    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 0.5, 3.5, in_iwidth_iheight);
    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 2.5, 2.5, in_iwidth_iheight);

    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 0.5, 7.5, in_iwidth_iheight);
    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 2.5, 5.5, in_iwidth_iheight);
    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 4.5, 3.5, in_iwidth_iheight);
    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 6.5, 1.5, in_iwidth_iheight);

    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 3.5, 11.5, in_iwidth_iheight);
    quadrant_sum = SumQuadrant(quadrant_sum, pivot, 10.5, 7.5, in_iwidth_iheight);

    float2 result = float2(
        (quadrant_sum.x + quadrant_sum.y + quadrant_sum.z + quadrant_sum.w) / 32.0,
        saturate((quadrant_sum.x - ((quadrant_sum.y + quadrant_sum.z + quadrant_sum.w) / 6.0)) / 8.0)
        );

    return result;
}

float4 CalculateColour(
    float2 in_uv,
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    float4 texel = g_texture.Sample(g_sampler_state, in_uv);

    float2 gloss_darken = CalculateGlossDark(
        in_uv,
        in_width_height,
        in_iwidth_iheight
        );

    // Premultipiled alpha
    //result = foreground + (1 - foreground.alpha) * background

    float darken = 1.0 - (gloss_darken.x * 1.25 * texel.a);
    float gloss = gloss_darken.y * gloss_darken.y * texel.a * 0.4;

    float4 result = float4(
        (texel.x * darken) + gloss,
        (texel.y * darken) + gloss,
        (texel.z * darken) + gloss,
        texel.a
        );
    return result;
}

Pixel main(Interpolant in_input)
{
    Pixel result;

    float4 colour = CalculateColour(
        in_input._uv, 
        float2(_width_height_iwidth_iheight.x, _width_height_iwidth_iheight.y),
        float2(_width_height_iwidth_iheight.z, _width_height_iwidth_iheight.w)
        );

    result._colour = colour;

    return result;
}