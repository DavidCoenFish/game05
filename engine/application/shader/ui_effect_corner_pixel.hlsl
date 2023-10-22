#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

float CalculateAlpha(float in_x, float in_y, float in_radius)
{
    float distance = sqrt((in_x * in_x) + (in_y * in_y));
    float coverage = 1.0 - saturate(distance - (in_radius - 0.5));
    return coverage;
}

float CalculateCornerAlpha(
    float2 in_uv, 
    float4 in_radius,
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    float2 pixel_pos = in_uv * in_width_height;

    float top_right = CalculateAlpha(
        max(0.0, in_radius.x - max(0.0, pixel_pos.y)),
        max(0.0, in_radius.x - max(0.0, in_width_height.x - pixel_pos.x)),
        in_radius.x
        );
    float top_left = CalculateAlpha(
        max(0.0, in_radius.y - max(0.0, pixel_pos.y)),
        max(0.0, in_radius.y - max(0.0, pixel_pos.x)),
        in_radius.y
        );
    float bottom_left = CalculateAlpha(
        max(0.0, in_radius.z - max(0.0, in_width_height.y - pixel_pos.y)),
        max(0.0, in_radius.z - max(0.0, pixel_pos.x)),
        in_radius.z
        );
    float bottom_right = CalculateAlpha(
        max(0.0, in_radius.w - max(0.0, in_width_height.y - pixel_pos.y)),
        max(0.0, in_radius.w - max(0.0, in_width_height.x - pixel_pos.x)),
        in_radius.w
        );

    return top_right * top_left * bottom_left * bottom_right;
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

    float corner_alpha = CalculateCornerAlpha(
        in_input._uv,
        _constant_buffer_1_data0,
        float2(_width_height_iwidth_iheight.x, _width_height_iwidth_iheight.y),
        float2(_width_height_iwidth_iheight.z, _width_height_iwidth_iheight.w)
        );

    Pixel result;

    // Premultipiled alpha
    result._colour = texel * corner_alpha;
    //result._colour = float4(corner_alpha, corner_alpha, corner_alpha, 1.0);
    //result._colour = float4(texel.rgb, texel.a * corner_alpha);

    return result;
}