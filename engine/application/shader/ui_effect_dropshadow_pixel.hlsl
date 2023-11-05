#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

float ClampSample(
    float2 in_uv
    )
{
    // this is exspencive, but zero out rather than wrap.
    //float ratio = step(0.0, in_uv.x) * step(0.0, in_uv.y) * step(in_uv.x, 1.0) * step(in_uv.y, 1.0);

    float texel = g_texture.Sample(g_sampler_state, in_uv).a;
    float result = texel; //(texel * ratio);
    return result;
}


float2 SampleAtOffset(
    float2 in_coverage_shadow,
    float2 in_pivot,
    float2 in_offset,
    float2 in_iwidth_iheight,
    float in_radius,
    float in_low,
    float in_high
    )
{
#if defined(GEOMETRY_SIZE_INTO_SHADER)
    float4 texel = GetBlockTexel(
        g_texture,
        g_sampler_state,
        _geometry_pos,
        _geometry_uv,
        in_uv
        );
#else
    float texel = 0.0;
    texel += ClampSample((in_pivot + in_offset) * in_iwidth_iheight);
    texel += ClampSample((in_pivot - in_offset) * in_iwidth_iheight);
    texel += ClampSample((in_pivot + float2(in_offset.y, -in_offset.x)) * in_iwidth_iheight);
    texel += ClampSample((in_pivot + float2(-in_offset.y, in_offset.x)) * in_iwidth_iheight);
#endif

    float ratio = saturate((in_radius - in_low) / (in_high - in_low));
    //float coverage = 4.0 * ratio;
    // [sum value, sum coverage]
    return float2(
        in_coverage_shadow.x + (texel * ratio / 4.0),
        in_coverage_shadow.y + ratio
        );
}

float CalculateShadowAlpha(
    float2 in_uv, 
    float2 in_offset,
    float in_radius,
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    float2 pivot = (floor((in_uv * in_width_height) - in_offset)) + float2(0.5, 0.5);

    // [sum value, sum coverage]
    float4 texel = ClampSample(pivot * in_iwidth_iheight);

    float2 coverage_shadow = float2(texel.a, 1.0);

    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(0.5, 1.5), in_iwidth_iheight, in_radius, 0.5, 2.9154);
    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(0.5, 3.5), in_iwidth_iheight, in_radius, 2.5, 5.1478);
    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(0.5, 5.5), in_iwidth_iheight, in_radius, 4.5, 6.6708);
    // following line disable to fit under 512 instructions
    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(2.5, 1.5), in_iwidth_iheight, in_radius, 1.5811, 4.3012);
    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(2.5, 3.5), in_iwidth_iheight, in_radius, 2.9154, 5.7009);
    coverage_shadow = SampleAtOffset(coverage_shadow, pivot, float2(4.5, 1.5), in_iwidth_iheight, in_radius, 3.5355, 6.0415);

    float result = coverage_shadow.x / coverage_shadow.y;
    return result;
}

Pixel main(Interpolant in_input)
{
#if defined(GEOMETRY_SIZE_INTO_SHADER)
    float4 texel = GetBlockTexel(
        g_texture,
        g_sampler_state,
        _geometry_pos,
        _geometry_uv,
        in_input._uv
        );
#else
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);
#endif

    float shadow_alpha = CalculateShadowAlpha(
        in_input._uv,
        float2(_constant_buffer_1_data0.x, _constant_buffer_1_data0.y),
        _constant_buffer_1_data0.z,
        float2(_width_height_iwidth_iheight.x, _width_height_iwidth_iheight.y),
        float2(_width_height_iwidth_iheight.z, _width_height_iwidth_iheight.w)
        );

    Pixel result;

    // Premultipiled alpha
    //result = foreground + (1 - foreground.alpha) * background
    result._colour = texel + ((1.0 - texel.a) * (_tint * shadow_alpha));

    return result;
}