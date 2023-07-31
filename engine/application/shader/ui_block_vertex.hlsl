#include "ui_block_interpolant.hlsli"

struct Vertex
{
    // [0 ... 1]
    float2 _position : Position;
};

// Should y be up or down? lets make it up?
cbuffer ConstantBuffer : register(b0)
{
    int4 _block_pos; // Low xy, high xy 
    float4 _block_uv; // Low xy, high xy [0 ... 1]
    int2 _target_size; // width, height of the thing we are drawing into
};

// Clamp to block to draw against the target
Interpolant main(Vertex in_input)
{
    Interpolant result;

    // [-1 ... 1]
    //float4 _position : SV_Position;
    int2 block_pos_low = max(int2(0,0), _block_pos.xy);
    int2 block_pos_high = min(_target_size, _block_pos.zw);

    float2 pos_low = float2(
        ((float)block_pos_low.x) / ((float)_target_size.x),
        ((float)block_pos_low.y) / ((float)_target_size.y)
        ) * 2.0 - 1.0;
    float2 pos_high = float2(
        ((float)block_pos_high.x) / ((float)_target_size.x),
        ((float)block_pos_high.y) / ((float)_target_size.y)
        ) * 2.0 - 1.0;
    float2 pos_delta = pos_high- pos_low;

    result._position = float4(
        pos_low.x + (pos_delta.x * in_input._position.x),
        pos_low.y + (pos_delta.y * in_input._position.y),
        0.0,
        1.0
        );

    // [0 ... 1]
    //float2 _uv : TEXCOORD0;
    float2 trim_low = float2(
        (float)(block_pos_low.x) / ((float)(_block_pos.z - _block_pos.x)),
        (float)(block_pos_low.y) / ((float)(_block_pos.w - _block_pos.y))
        );
    float2 trim_high = float2(
        (float)(block_pos_high.x) / ((float)(_block_pos.z - _block_pos.x)),
        (float)(block_pos_high.y) / ((float)(_block_pos.w - _block_pos.y))
        );

    float2 uv_low = _block_uv.xy * trim_low;
    float2 uv_high = _block_uv.zw * trim_high;
    float2 uv_delta = uv_high - uv_low;

    result._uv = uv_low + (uv_delta * in_input._position.xy);

    return result;
}
