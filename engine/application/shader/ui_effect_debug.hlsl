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

Pixel main(Interpolant in_input)
{
    float4 texel = GetBlockDebug(
        g_texture,
        g_sampler_state,
        _geometry_pos,
        _geometry_uv,
        in_input._uv
        );

    Pixel result;

    result._colour = texel;
    //result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);

    return result;
}