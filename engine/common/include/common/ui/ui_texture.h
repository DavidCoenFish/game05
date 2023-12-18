#pragma once

#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class DrawSystemFrame;
class HeapWrapperItem;
class IResource;
class IRenderTarget;
class RenderTargetTexture;
class Shader;
class VectorInt2;

/// Own a render target texture, or use the draw system back buffer
class UITexture
{
public:
    UITexture(
        const bool in_draw_to_texture = false,
        const bool in_always_dirty = false,
        const bool in_allow_clear = false,
        const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    ~UITexture();

    const bool SetRenderTarget(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame
        );

    const VectorInt2 GetSize(
        DrawSystem* const in_draw_system
        ) const;
    const bool GetDrawToTexture() const { return _draw_to_texture; }

    const bool Update(
        const bool in_draw_to_texture = false,
        const bool in_always_dirty = false,
        const bool in_allow_clear = false,
        const VectorFloat4& in_clear_colour = VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );

    const bool SetSize(
        const VectorInt2& in_size
        );

    void MarkDirty()
    {
        _has_drawn = false;
    }

    void SetShaderResource(
        Shader& in_shader,
        const int in_index,
        DrawSystemFrame* const in_draw_system_frame
        );

    //const bool GetHasDrawn() const { return _has_drawn; }
    void SetHasDrawn(const bool in_has_drawn) { _has_drawn = in_has_drawn; return; }

    const bool GetAllowClear() const { return _allow_clear; }
    //const bool GetAlwaysDirty() const { return _always_dirty; }

    const bool CalculateNeedsToDraw() const;

private:
    void UpdateRenderTarget(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );

private:
    /// true == use _render_target_texture else use the application backbuffer via draw system
    bool _draw_to_texture; 
    /// When render target is set active, do we clear
    bool _allow_clear; 
    /// If we share a render target, like the backbuffer, we may need to always draw
    bool _always_dirty; 
    /// Internal flag to help know if data drawn to texture
    bool _has_drawn; 

    // resizing the render target seems to be triggering a object deleted while still in use error
    //bool _needs_resize;
    VectorInt2 _size;
    VectorFloat4 _clear_colour;

    std::shared_ptr<RenderTargetTexture> _render_target_texture;

};
