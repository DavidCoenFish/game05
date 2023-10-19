#if defined(GEOMETRY_SIZE_INTO_SHADER)

float4 GetBlockTexel(
    Texture2D in_texture,
    SamplerState in_sampler_state,
    float4 in_geometry_pos, // -1 ... 1
    float4 in_geometry_uv, // 0 ... 1
    float2 in_uv
    )
{
    float2 uv_geom_space = (in_uv * 2.0) - 1.0;
    if ((uv_geom_space.x < in_geometry_pos.x) ||
        (in_geometry_pos.z < uv_geom_space.x) ||
        (uv_geom_space.y < in_geometry_pos.y) ||
        (in_geometry_pos.w < uv_geom_space.y))
    {
        return float4(0.0, 0.0, 0.0, 0.0);
    }
    float2 geometry_ratio = (uv_geom_space - float2(in_geometry_pos.x, in_geometry_pos.y)) / 
        float2(in_geometry_pos.z - in_geometry_pos.x, in_geometry_pos.w - in_geometry_pos.y);
    float2 uv = lerp(float2(in_geometry_uv.x, in_geometry_uv.w), float2(in_geometry_uv.z, in_geometry_uv.y), geometry_ratio);
    float4 result = in_texture.Sample(in_sampler_state, uv);
    return result;
}

float4 GetBlockDebug(
    Texture2D in_texture,
    SamplerState in_sampler_state,
    float4 in_geometry_pos, // -1 ... 1
    float4 in_geometry_uv, // 0 ... 1
    float2 in_uv
    )
{
    float2 uv_geom_space = (in_uv * 2.0) - 1.0;
    if ((uv_geom_space.x < in_geometry_pos.x) ||
        (in_geometry_pos.z < uv_geom_space.x) ||
        (uv_geom_space.y < in_geometry_pos.y) ||
        (in_geometry_pos.w < uv_geom_space.y))
    {
        return float4(0.0, 0.0, 0.0, 0.0);
    }
    float2 geometry_ratio = (uv_geom_space - float2(in_geometry_pos.x, in_geometry_pos.y)) / 
        float2(in_geometry_pos.z - in_geometry_pos.x, in_geometry_pos.w - in_geometry_pos.y);
    float2 uv = lerp(float2(in_geometry_uv.x, in_geometry_uv.w), float2(in_geometry_uv.z, in_geometry_uv.y), geometry_ratio);
    float4 result = float4(uv.x, uv.y, 0.0, 1.0);
    return result;
}

#endif