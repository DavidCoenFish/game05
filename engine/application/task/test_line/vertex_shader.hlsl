struct Vertex
{
    float2 _position : Position;
};

// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
struct Interpolant
{
    float4 _position : SV_Position;
    float2 _uv : TEXCOORD0;
};

Interpolant main(Vertex in_input)
{
    Interpolant result;
    result._position = float4(in_input._position.x, in_input._position.y, 0.0f, 1.0f);
    result._uv = float2(in_input._position.x, in_input._position.y);
    return result;
}