#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

// Todo: rename ui_effect_chrome_pixel.hlsl
// Warning: respects alpha from input texture, but paints over all non ampha pixels, 
// ie, this shader "fills" the solid area of a ui element

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

float SumQuadrant(float in_sum, float2 in_pivot, float in_a, float in_b, float2 in_iwidth_iheight)
{
    float result = in_sum +
        SumQuadrantSample(in_pivot, float2(-in_a, -in_b), in_iwidth_iheight) + 
        SumQuadrantSample(in_pivot, float2(in_b, -in_a), in_iwidth_iheight) + 
        SumQuadrantSample(in_pivot, float2(in_a, in_b), in_iwidth_iheight) +
        SumQuadrantSample(in_pivot, float2(-in_b, in_a), in_iwidth_iheight);
    return result;
}

float CalculateBlur(
    float2 in_uv,
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    float2 pivot = (floor((in_uv * in_width_height) + float2(0.05, 0.05))) + float2(0.5, 0.5);
    //float2 pivot = in_uv * in_width_height;

    float sum = 0.0;
    //sum = SumQuadrant(sum, pivot, 0.5, 3.5, in_iwidth_iheight);
    //sum = SumQuadrant(sum, pivot, 2.5, 2.5, in_iwidth_iheight);

    sum = SumQuadrant(sum, pivot, 0.5, 7.5, in_iwidth_iheight);
    sum = SumQuadrant(sum, pivot, 2.5, 5.5, in_iwidth_iheight);
    sum = SumQuadrant(sum, pivot, 4.5, 3.5, in_iwidth_iheight);
    sum = SumQuadrant(sum, pivot, 6.5, 1.5, in_iwidth_iheight);

    float result = sum / 16.0f;
    return result;
}

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    float blur = CalculateBlur(
        in_input._uv, 
        float2(_width_height_iwidth_iheight.x, _width_height_iwidth_iheight.y),
        float2(_width_height_iwidth_iheight.z, _width_height_iwidth_iheight.w)
        );

    float ratio = in_input._uv.y - (blur * _data.x * _width_height_iwidth_iheight.w);

    //_tint
    float4 _tint_light = float4(1.0 - (_tint.xyz * 0.4), _tint.a);
    float4 _tint_invert = float4((1.0 - _tint.xyz) * 0.3, _tint.a);

    float u = smoothstep(0.495, 0.505, ratio);

    float t2 = ratio * 2.0;
    //t2 *= t2;
    t2 = saturate(t2);
    float4 upper = lerp(_tint, _tint_light, t2);

    float t1 = 2.0 - (ratio * 2.0);
    //t1 *= t1;
    t1 = saturate(1.0 - t1);
    float4 lower = lerp(_tint_invert, _tint_light, t1);

    float4 colour = lerp(upper, lower, u);

    // Premultipiled alpha
    //result = foreground + (1 - foreground.alpha) * background

    Pixel result;
    //result._colour = float4(0.0, 0.0, 1.0, 1.0);
    result._colour = colour * texel.a;
    //result._colour = float4(u, u, u, 1.0);
    //result._colour = lower * texel.a;
    //result._colour = _tint * texel.a;

    return result;
}