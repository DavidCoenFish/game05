// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
struct Interpolant
{
    float4 _position : SV_Position;
    float2 _uv : TEXCOORD0;
    float3 _mask : TEXCOORD1;
};