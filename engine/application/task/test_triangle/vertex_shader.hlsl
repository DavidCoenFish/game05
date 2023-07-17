struct Vertex
{
    float2 position : Position;
    float4 color : COLOR0;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

VertexOutput main( Vertex input )
{
   VertexOutput output;
   output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
   output.color = input.color;
   return output;
}