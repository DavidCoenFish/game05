// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
struct Interpolant
{
    float4 _position : SV_Position;
    float2 _uv : TEXCOORD0; // duplicate position as pos semantics don't interpolate
    float4 _line_pos_thickness : POSITION1;
    float4 _line_dir_length : POSITION2;
    float4 _line_colour : COLOR1;
};