#include "ui_interpolant.hlsli"
#include "ui_common_pixel.hlsli"
#include "ui_effect_common_pixel.hlsli"

Pixel main(Interpolant in_input)
{
#if defined(GEOMETRY_SIZE_INTO_SHADER)
    float4 texel = GetBlockDebug(
        g_texture,
        g_sampler_state,
        _geometry_pos,
        _geometry_uv,
        in_input._uv
        );
#endif
    Pixel result;

    //result._colour = texel;
    result._colour = float4(in_input._uv.x, in_input._uv.y, 0.0, 1.0);

    return result;
}