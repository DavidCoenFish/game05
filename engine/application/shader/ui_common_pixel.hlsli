Texture2D g_texture : register(t0);
SamplerState g_sampler_state : register(s0);

cbuffer ConstantBufferBlock : register(b0)
{
    float4 _tint;
};

struct Pixel
{
    float4 _colour : SV_TARGET0;
};
