#pragma once

#include "common/ui/ui_component/i_ui_component.h"

#include "common/ui/ui_enum.h"
#include "common/math/vector_float4.h"

enum class UIEffectEnum;
class ShaderConstantBuffer;
struct UIHierarchyNodeUpdateParam;
struct UIManagerDrawParam;

/// Initially thought effect would have multiple shader inputs, but is one enought?
/// if we want more than one shader input, switch count based on UIEffectEnum? (or make a new class?)
/// geometry is a pos(-1,-1 1,1) uv(0,1 1,0) which can be shared in the UIManager?
class UIEffectComponent
{
public:
    typedef std::array<VectorFloat4, static_cast<int>(UIStateFlag::TTintPermutationCount)> TStateFlagTintArray;

    /// the generic data for the effect shaders
    struct TShaderConstantBuffer
    {
        VectorFloat4 _width_height_iwidth_iheight;
        VectorFloat4 _data;
        VectorFloat4 _tint;
    };

    UIEffectComponent(
        const UIEffectEnum in_type,
        const UICoord& in_coord_a,
        const UICoord& in_coord_b,
        const UICoord& in_coord_c,
        const UICoord& in_coord_d,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null = nullptr,
        const VectorFloat4& in_default_tint = VectorFloat4::s_white
        );
    /// destructed by base type IUIComponent which has virtual dtor, so virtual here may be redundant but does provide info
    virtual ~UIEffectComponent();

    /// return true if modified, else false
    const bool Set(
        const UIEffectEnum in_type,
        const UICoord& in_coord_a,
        const UICoord& in_coord_b,
        const UICoord& in_coord_c,
        const UICoord& in_coord_d,
        const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array_or_null = nullptr,
        const VectorFloat4& in_default_tint = VectorFloat4::s_white
        );

    void Update(
        const UIHierarchyNodeUpdateParam& in_param,
        const VectorInt2& in_target_size
        );

    /// Given the input UITexture as a shader resource input, we draw to our own texture using an effect shader
    void Render(
        const UIManagerDrawParam& in_draw_param,
        UITexture& in_input_texture,
        const UIStateFlag in_state_flag
        );

    UITexture& GetUITexture() const { return *_texture; }

private:
    /// The shader constants for this effect
    std::shared_ptr<ShaderConstantBuffer> _shader_constant_buffer;

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

    /// optional array of tint colours selected via input state flag in Render
    std::shared_ptr<const TStateFlagTintArray> _state_flag_tint_array_or_null;

    /// tint colour to use if _state_flag_tint_array_or_null is null
    VectorFloat4 _default_tint;

    /// The render target we draw to and then the shader resource (texture) that others consume
    std::unique_ptr<UITexture> _texture;

};
