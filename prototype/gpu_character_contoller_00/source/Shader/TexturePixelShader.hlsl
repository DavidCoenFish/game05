struct Interpolant
{
    float4 position     : SV_Position;
    float2 uv           : TEXCOORD0;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

Pixel main( Interpolant input )
{
    Pixel Out;
    Out.color = txDiffuse.Sample(samLinear, input.uv);
    return Out;
}