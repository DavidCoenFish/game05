struct Vertex
{
    float2 position     : Position;
    float2 uv           : TEXCOORD0;
};

struct Interpolant
{
    float4 position     : SV_Position;
    float2 uv           : TEXCOORD0;
};

Interpolant main( Vertex input )
{
   Interpolant output;
   output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
   output.uv = input.uv;
   return output;
}