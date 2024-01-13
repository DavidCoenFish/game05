// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
struct Interpolant
{
    float4 _position : SV_Position;
    float2 _uv : TEXCOORD0;
    float2 _p0 : COLOR0;
    float2 _p1 : COLOR1;
    float2 _p2 : COLOR2;
};