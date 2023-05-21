struct Interpolants
{
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

Pixel main( Interpolants In )
{
    Pixel Out;
    Out.color = In.color;
    return Out;
}