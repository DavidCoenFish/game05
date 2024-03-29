#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

float ClampSample(
    float2 in_uv
    )
{
    // this is exspencive, but zero out rather than wrap.
    float ratio = step(0.0, in_uv.x) * step(0.0, in_uv.y) * step(in_uv.x, 1.0) * step(in_uv.y, 1.0);

    float texel_a = g_texture.Sample(g_sampler_state, in_uv).a;
    float result = 1.0 - (texel_a * ratio);
    return result;
}


float2 SampleAtOffset(
    float2 in_coverage_shadow,
    float2 in_pivot,
    float2 in_offset,
    float2 in_i_width_i_height
    )
{
    float texel = 0.0;
    texel += ClampSample((in_pivot + in_offset) * in_i_width_i_height);
    texel += ClampSample((in_pivot - in_offset) * in_i_width_i_height);
    texel += ClampSample((in_pivot + float2(in_offset.y, -in_offset.x)) * in_i_width_i_height);
    texel += ClampSample((in_pivot + float2(-in_offset.y, in_offset.x)) * in_i_width_i_height);

    // [sum value, sum coverage]
    return float2(
        in_coverage_shadow.x + (texel / 4.0),
        in_coverage_shadow.y + 1.0
        );
}

float CalculateShadowAlpha(
    float2 in_uv, 
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    float2 pivot = floor(in_uv * in_width_height) + float2(0.5, 0.5);

    // [sum value, sum coverage]
    float2 coverage_shadow = float2(0.0, 0.0);

    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(0.5, 1.5), in_iwidth_iheight);
    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(0.5, 3.5), in_iwidth_iheight);
    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(2.5, 1.5), in_iwidth_iheight);

    //coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(0.5, 5.5), in_iwidth_iheight);
    //coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(2.5, 3.5), in_iwidth_iheight);
    //coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(0.5, 1.5), in_iwidth_iheight);

    float result = coverage_shadow.x / coverage_shadow.y;
    return result;
}

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    float shadow_alpha = CalculateShadowAlpha(
        in_input._uv,
        float2(_width_height_iwidth_iheight.x, _width_height_iwidth_iheight.y),
        float2(_width_height_iwidth_iheight.z, _width_height_iwidth_iheight.w)
        );

    Pixel result;

    float4 shadow = _tint * shadow_alpha * texel.a;

    // Premultipiled alpha
    //result = foreground + (1 - foreground.alpha) * background
    result._colour = shadow + ((1.0 - shadow.a) * texel);
    //result._colour = float4(1.0, 0.0, 0.0, 1.0);
    //result._colour = float4(shadow_alpha, shadow_alpha, shadow_alpha, 1.0);

    return result;
}