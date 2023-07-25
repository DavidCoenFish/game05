cbuffer Constants : register(b0)
{
    float4 g_timer_thickness : packoffset(c0);
    float4 g_colour : packoffset(c1);
};

RWTexture2D<float4> _output_texture_line_pos_thickness : register(u0);
RWTexture2D<float4> _output_texture_line_dir_length : register(u1);
RWTexture2D<float4> _output_texture_line_colour : register(u2);

float3 GetAtomixPos(uint in_trunc, float in_frac)
{
    uint axis0 = in_trunc / 2;
    float3 result = float3(0.0, 0.0, 0.0);
    float v0 = 1.0 - in_frac;
    float v1 = 1.0;
    if (0 == (in_trunc % 2))
    {
        v0 = 1.0;
        v1 = in_frac;
    }

    if (0 == axis0)
    {
        result.x = v0;
        result.y = v1;
    }
    else if (1 == axis0)
    {
        result.y = v0;
        result.z = v1;
    }
    else if (2 == axis0)
    {
        result.z = v0;
        result.x = v1;
    }

    return result;
}

float3 DealQuadrant(uint in_quadrant, float3 in_atomic_pos)
{
    uint x_y_index = in_quadrant % 4;
    float3 y_axis = float3(0.0, 1.0, 0.0);
    float3 x_axis = float3(0.0, 0.0, 0.0);
    float3 z_axis = float3(0.0, 0.0, 0.0);
    if (4 <= in_quadrant)
    {
        y_axis.y = -1.0;
    }
    if (0 == x_y_index)
    {
        x_axis.x = 1.0;
        z_axis.z = 1.0;
    }
    else if (1 == x_y_index)
    {
        x_axis.z = -1.0;
        z_axis.x = 1.0;
    }
    else if (2 == x_y_index)
    {
        x_axis.x = -1.0;
        z_axis.z = -1.0;
    }
    else if (3 == x_y_index)
    {
        x_axis.z = 1.0;
        z_axis.x = -1.0;
    }

    float3 result = float3(
        dot(in_atomic_pos, x_axis),
        dot(in_atomic_pos, y_axis),
        dot(in_atomic_pos, z_axis)
        );
    return result;
}

float3 MakePos(uint in_index, float2 in_timer)
{
    uint quadrant = in_index / 3;
    uint offset = in_index % 3;
    uint atomic_trunc = (((uint)in_timer.x) + (offset * 2)) % 6;
    float3 atomic_pos = GetAtomixPos(atomic_trunc, in_timer.y);
    float3 result = DealQuadrant(quadrant, atomic_pos);
    return result;
}

float3 MakePosNext(uint in_index, float2 in_timer)
{
    uint quadrant = in_index / 3;
    uint offset = in_index % 3;
    uint atomic_trunc = (((uint)in_timer.x) + 2 + (offset * 2)) % 6;
    float3 atomic_pos = GetAtomixPos(atomic_trunc, in_timer.y);
    float3 result = DealQuadrant(quadrant, atomic_pos);
    return result;
}

float4 MakeDir(uint in_index, float2 in_timer, float3 in_line_pos)
{
    float3 line_pos_next = MakePosNext(in_index, in_timer);
    float3 dir = line_pos_next - in_line_pos;
    float result_length = length(dir);
    float4 result = float4(dir / result_length, result_length);
    return result;
}

// Each "square" takes 24 lines, texture size for [8,4,8] is [24 * 8, 4 * 8]? 192x32
[numthreads(8, 4, 8)]
void main(uint3 in_id : SV_DispatchThreadID)
{
    float3 origin = float3(in_id.x * 3.0, in_id.y * 2.0, in_id.z * 3.0);
    float timer_temp = g_timer_thickness.x * 6.0;
    float2 timer = float2(trunc(timer_temp), frac(timer_temp));
    float3 line_pos;
    float4 line_dir_length;
    uint2 output_uv;

    for (uint index = 0; index < 24; ++index)
    {
        line_pos = MakePos(index, timer);
        line_dir_length = MakeDir(index, timer, line_pos);

        output_uv = uint2(in_id.x * index, in_id.y * in_id.z);
        _output_texture_line_pos_thickness[output_uv] = float4(line_pos + origin, g_timer_thickness.y);
        _output_texture_line_dir_length[output_uv] = line_dir_length;
        _output_texture_line_colour[output_uv] = g_colour;
    }
}