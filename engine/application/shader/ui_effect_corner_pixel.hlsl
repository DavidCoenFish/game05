#include "ui_block_interpolant.hlsli"
#include "ui_effect_common.hlsli"

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
    float4 _top_right_top_left_bottom_left_bottom_right; //radius
    float4 _width_height_iwidth_iheight;
    float4 _geometry_pos;
    float4 _geometry_uv;
};

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
    //float left = max(0.0, pixel_pos.x);
    //left = left * left;
    //float right = max(0.0, in_width_height.x - pixel_pos.x);
    //right = right * right;
    //float top = max(0.0, pixel_pos.y);
    //top = top * top;
    //float bottom =  max(0.0, in_width_height.y - pixel_pos.y);
    //bottom = bottom * bottom;

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
    //float4 texel = g_texture.Sample(g_sampler_state, in_input._uv);
    float4 texel = GetBlockTexel(
        g_texture,
        g_sampler_state,
        _geometry_pos,
        _geometry_uv,
        in_input._uv
        );

    float corner_alpha = CalculateCornerAlpha(
        in_input._uv,
        _top_right_top_left_bottom_left_bottom_right,
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