// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
struct Interpolant
{
    // [-1 ... 1]
    float4 _position : SV_Position;

    // [0 .. 1]
    float2 _uv : TEXCOORD0;

};