#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

float CalculateCornerAlpha(float in_x, float in_y, float in_radius)
{
    float distance = sqrt((in_x * in_x) + (in_y * in_y));
    float coverage = 1.0 - saturate(distance - (in_radius - 0.5));
    return coverage;
}

float CalculateAlpha(
    float2 in_uv, 
    float4 in_radius,
    float2 in_width_height,
    float2 in_iwidth_iheight
    )
{
    float2 pixel_pos = in_uv * in_width_height;

    float top_right = CalculateCornerAlpha(
        max(0.0, in_radius.x - max(0.0, pixel_pos.y)),
        max(0.0, in_radius.x - max(0.0, in_width_height.x - pixel_pos.x)),
        in_radius.x
        );
    float top_left = CalculateCornerAlpha(
        max(0.0, in_radius.y - max(0.0, pixel_pos.y)),
        max(0.0, in_radius.y - max(0.0, pixel_pos.x)),
        in_radius.y
        );
    float bottom_left = CalculateCornerAlpha(
        max(0.0, in_radius.z - max(0.0, in_width_height.y - pixel_pos.y)),
        max(0.0, in_radius.z - max(0.0, pixel_pos.x)),
        in_radius.z
        );
    float bottom_right = CalculateCornerAlpha(
        max(0.0, in_radius.w - max(0.0, in_width_height.y - pixel_pos.y)),
        max(0.0, in_radius.w - max(0.0, in_width_height.x - pixel_pos.x)),
        in_radius.w
        );

    float hollow = 1.0 - ((1.0 - step(pixel_pos.x, in_radius.x)) 
        * (1.0 - step(pixel_pos.y, in_radius.y))
        * (1.0 - step(in_width_height.x - in_radius.z, pixel_pos.x))
        * (1.0 - step(in_width_height.y - in_radius.w, pixel_pos.y))
        );

    return top_right * top_left * bottom_left * bottom_right * hollow;
}

Pixel main(Interpolant in_input)
{
    float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);

    float alpha = CalculateAlpha(
        in_input._uv,
        _data,
        float2(_width_height_iwidth_iheight.x, _width_height_iwidth_iheight.y),
        float2(_width_height_iwidth_iheight.z, _width_height_iwidth_iheight.w)
        );

    Pixel result;

    // Premultipiled alpha
    result._colour = texel * alpha;
    //result._colour = float4(corner_alpha, corner_alpha, corner_alpha, 1.0);
    //result._colour = float4(texel.rgb, texel.a * corner_alpha);

    return result;
}