#pragma once

#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

class DrawSystem;
class HeapWrapperItem;
class IRenderTarget;
class RenderTargetTexture;
class VectorInt2;
/*
    Own a render target texture, 
    or reference the back buffer or other external texture render target
*/
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

    IRenderTarget* GetRenderTarget(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
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

    void SetSize(
        //DrawSystem* const in_draw_system,
        //ID3D12GraphicsCommandList* const in_command_list,
        const VectorInt2& in_size
        );

    void MarkDirty()
    {
        _has_drawn = false;
    }

    // nullptr if _bUseBackBuffer is true
    std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    const bool GetHasDrawn() const { return _has_drawn; }
    void SetHasDrawn(const bool in_has_drawn) { _has_drawn = in_has_drawn; return; }

    const bool GetAllowClear() const { return _allow_clear; }
    const bool GetAlwaysDirty() const { return _always_dirty; }

private:
    bool _draw_to_texture; // true == use _render_target_texture else use the application backbuffer via draw system
    bool _allow_clear; // When render target is set active, do we clear
    bool _always_dirty; // If we share a render target, like the backbuffer, we may need to always draw
    bool _has_drawn; // Internal flag to help know if data drawn to texture
    VectorInt2 _size;
    VectorFloat4 _clear_colour;

    std::shared_ptr<RenderTargetTexture> _render_target_texture;
    bool _render_target_dirty;

};
