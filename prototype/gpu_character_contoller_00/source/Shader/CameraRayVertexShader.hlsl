struct Vertex
{
    float2 position     : Position;
    float2 uv	        : TEXCOORD0;
};

struct VertexOutput
{
    float4 position     : SV_Position;
    float2 uv           : TEXCOORD0;
};

VertexOutput main( Vertex input )
{
   VertexOutput output;
   output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
   output.uv = input.uv;
   return output;
}