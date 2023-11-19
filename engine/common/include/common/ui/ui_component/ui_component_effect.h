#pragma once

#include "common/ui/ui_component/i_ui_component.h"

#include "common/ui/ui_enum.h"
#include "common/math/vector_float4.h"

enum class UIEffectEnum;
class ShaderConstantBuffer;
struct UIHierarchyNodeUpdateLayoutRenderParam;

// passing geometry info into the shader worked, but is rather expensive on the shader, drop shadow had more than 512 instructions
// #define GEOMETRY_SIZE_INTO_SHADER

/// initially thought effect would have multiple shader inputs, but is one enought?
/// if we want more than one shader input, switch count based on UIEffectEnum? (or make a new class?)
class UIComponentEffect : public IUIComponent
{
    typedef IUIComponent TSuper;
public:
    /// the generic data for the effect shaders
    struct TShaderConstantBuffer
    {
        VectorFloat4 _data;
        VectorFloat4 _width_height_iwidth_iheight;
#if defined(GEOMETRY_SIZE_INTO_SHADER)
        VectorFloat4 _geometry_pos;
        VectorFloat4 _geometry_uv;
#endif
    };

    UIComponentEffect(
        const UIEffectEnum in_type,
        const UICoord& in_coord_a,
        const UICoord& in_coord_b,
        const UICoord& in_coord_c,
        const UICoord& in_coord_d,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_array_or_null = nullptr
        );
    /// destructed by base type IUIComponent which has virtual dtor, so virtual here may be redundant but does provide info
    virtual ~UIComponentEffect();

    /// return true if modified, else false
    const bool Set(
        const UIEffectEnum in_type,
        const UICoord& in_coord_a,
        const UICoord& in_coord_b,
        const UICoord& in_coord_c,
        const UICoord& in_coord_d,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_array_or_null = nullptr
        );

    const bool SetContainerSize(
        const VectorInt2& in_texture_size,
        const UIHierarchyNodeUpdateLayoutRenderParam& in_param
        );

private:
    /// deal with the component being drawn to the node texture
    virtual const bool PreDraw(
        const UIManagerDrawParam& in_draw_param,
        UIHierarchyNode& in_node
        ) override;

private:
    /// The shader constants for this effect
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

#if defined(GEOMETRY_SIZE_INTO_SHADER)
    /// full size to the render target draw surface
    /// we don't use the child component geometry as we may want to draw outside it, ie, dropshadow
    std::unique_ptr<UIGeometry> _geometry;
#endif

    /// The following could be kept in UIData, but trying to decouple UIData from [input,render]

    /// either we don't use type none as default ui shader, or need to be carefull with type to use with _shader_constant_buffer
    UIEffectEnum _type;

    /// used to build the shader constants
    UICoord _coord_a;
    /// used to build the shader constants
    UICoord _coord_b;
    /// used to build the shader constants
    UICoord _coord_c;
    /// used to build the shader constants
    UICoord _coord_d;

};
