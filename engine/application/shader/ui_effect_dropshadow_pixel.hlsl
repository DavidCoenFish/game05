#include "ui_block_interpolant.hlsli"

struct Pixel
{
    float4 _colour : SV_TARGET0;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler_state : register(s0);

cbuffer ConstantBufferBlock : register(b0)
{
    float4 _tint;
};

cbuffer ConstantBufferEffect : register(b1)
{
    float4 _offset_x_y_radius;
    float4 _width_height_iwidth_iheight;
};

float SampleAtOffset(
    float2 in_coverage_shadow,
    float2 in_uv,
    float in_radius,
    float in_low,
    float in_high
    )
{
    float4 texel = g_texture.Sample(g_sampler_state, in_uv);
    float ratio = 1.0 - saturate((in_radius - in_low) / (in_high - in_low));
    float coverage = 4.0 * ratio;
    float new_area_sum = in_coverage_shadow.y + coverage;
    float new_value = ((in_coverage_shadow.x * in_coverage_shadow.y) + (texel.a * coverage)) / new_area_sum;
    // [averaged value, area_sum]
    //return float2(
    //    new_value,
    //    new_area_sum
    //    );
    return texel.a;
}

float CalculateShadowAlpha(
    float2 in_uv, 
    float2 in_offset,
    float in_radius,
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    //float2 pivot = ((in_uv * in_width_height) - in_offset);
    // the extra +[0.5,0.5] inside the round is to avoid a horible (accuracy?) step
    float2 pivot = (round((in_uv * in_width_height) - in_offset + float2(0.5, 0.5))) + float2(0.5, 0.5);

    float4 texel = g_texture.Sample(g_sampler_state, pivot * in_iwidth_iheight);
    // [averaged value, area_sum]
    float2 coverage_shadow = float2(texel.a, 1.0);

    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(0.5, 1.5)) * in_iwidth_iheight, in_radius, 0.5, 2.9154);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(0.5, 3.5)) * in_iwidth_iheight, in_radius, 2.5, 5.1478);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(0.5, 5.5)) * in_iwidth_iheight, in_radius, 4.5, 6.6708);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(2.5, 1.5)) * in_iwidth_iheight, in_radius, 1.5811, 4.3012);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(2.5, 3.5)) * in_iwidth_iheight, in_radius, 2.9154, 5.7009);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(4.5, 1.5)) * in_iwidth_iheight, in_radius, 3.5355, 6.0415);

    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-1.5, 0.5)) * in_iwidth_iheight, in_radius, 0.5, 2.9154);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-3.5, 0.5)) * in_iwidth_iheight, in_radius, 2.5, 5.1478);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-5.5, 0.5)) * in_iwidth_iheight, in_radius, 4.5, 6.6708);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-1.5, 2.5)) * in_iwidth_iheight, in_radius, 1.5811, 4.3012);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-3.5, 2.5)) * in_iwidth_iheight, in_radius, 2.9154, 5.7009);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-1.5, 4.5)) * in_iwidth_iheight, in_radius, 3.5355, 6.0415);

    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-0.5, -1.5)) * in_iwidth_iheight, in_radius, 0.5, 2.9154);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-0.5, -3.5)) * in_iwidth_iheight, in_radius, 2.5, 5.1478);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-0.5, -5.5)) * in_iwidth_iheight, in_radius, 4.5, 6.6708);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-2.5, -1.5)) * in_iwidth_iheight, in_radius, 1.5811, 4.3012);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-2.5, -3.5)) * in_iwidth_iheight, in_radius, 2.9154, 5.7009);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(-4.5, -1.5)) * in_iwidth_iheight, in_radius, 3.5355, 6.0415);

    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(1.5, -0.5)) * in_iwidth_iheight, in_radius, 0.5, 2.9154);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(3.5, -0.5)) * in_iwidth_iheight, in_radius, 2.5, 5.1478);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(5.5, -0.5)) * in_iwidth_iheight, in_radius, 4.5, 6.6708);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(1.5, -2.5)) * in_iwidth_iheight, in_radius, 1.5811, 4.3012);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(3.5, -2.5)) * in_iwidth_iheight, in_radius, 2.9154, 5.7009);
    coverage_shadow = SampleAtOffset(coverage_shadow, (pivot + float2(1.5, -4.5)) * in_iwidth_iheight, in_radius, 3.5355, 6.0415);

    float result = coverage_shadow.x;
    return result;
}

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    float shadow_alpha = CalculateShadowAlpha(
        in_input._uv,
        float2(_offset_x_y_radius.x, _offset_x_y_radius.y),
        _offset_x_y_radius.z,
        float2(_width_height_iwidth_iheight.x, _width_height_iwidth_iheight.y),
        float2(_width_height_iwidth_iheight.z, _width_height_iwidth_iheight.w)
        );

    //shadow_alpha *= _tint.a;
    //shadow_alpha *= 1.0 - texel.a;

    Pixel result;
    //result._colour = float4(
    //    ((texel.rgb / texel.a) + ((_tint.rgb / _tint.a) * shadow_alpha)) * (shadow_alpha + texel.a),
    //    (shadow_alpha + texel.a)
    //    );

    //result._colour = texel;
    result._colour = float4(shadow_alpha,shadow_alpha,shadow_alpha,1.0);

    return result;
}