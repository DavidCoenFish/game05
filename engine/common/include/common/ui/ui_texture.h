#pragma once

#include "common/math/vector_int2.h"

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
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorInt2& in_size_for_not_backbuffer = VectorInt2(0,0),
        bool in_use_back_buffer = true
        );
    ~UITexture();

    IRenderTarget* GetRenderTarget() const;
    const VectorInt2 GetSize() const;
    const bool GetUseBackBuffer() const { return _use_back_buffer; }

    void SetSize(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorInt2& in_size
        );

    // nullptr if _bUseBackBuffer is true
    std::shared_ptr<HeapWrapperItem> GetShaderResourceHeapWrapperItem() const;

    const bool GetHasDrawn() const { return _has_drawn; }
    void SetHasDrawn(const bool in_has_drawn) { _has_drawn = in_has_drawn; return; }

private:
    //IRenderTarget* _render_target;
    bool _use_back_buffer;
    bool _has_drawn;
    DrawSystem* const _draw_system;

    // If _bUseBackBuffer is false, we own out own render target texture
    std::shared_ptr<RenderTargetTexture> _render_target_texture;

};
