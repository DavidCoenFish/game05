#include "common/common_pch.h"
#include "common/ui/ui_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/render_target/render_target_texture.h"

UITexture::UITexture(
    const bool in_draw_to_texture,
    const bool in_always_dirty,
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
    : _draw_to_texture(in_draw_to_texture)
    , _allow_clear(in_allow_clear)
    , _always_dirty(in_always_dirty)
    , _has_drawn(false)
    , _clear_colour(in_clear_colour)
{
    // Nop
}

UITexture::~UITexture()
{
    // Nop
}

IRenderTarget* UITexture::GetRenderTarget(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    if (true == _draw_to_texture)
    {
        if (nullptr == _render_target_texture)
        {
            _has_drawn = false;
            const std::vector<RenderTargetFormatData> target_format_data_array({
                RenderTargetFormatData(
                    DXGI_FORMAT_B8G8R8A8_UNORM, 
                    _allow_clear,
                    _clear_colour
                    )
                });

            _render_target_texture = in_draw_system->MakeRenderTargetTexture(
                in_command_list,
                target_format_data_array,
                RenderTargetDepthData(),
                _size
                );
        }

        return _render_target_texture.get();
    }
    return in_draw_system->GetRenderTargetBackBuffer();
}

const VectorInt2 UITexture::GetSize(
    DrawSystem* const in_draw_system
    ) const
{
    if (true == _draw_to_texture)
    {
        return _size;
    }
    return in_draw_system->GetRenderTargetBackBuffer()->GetSize();
}

const bool UITexture::Update(
    const bool in_draw_to_texture,
    const bool in_always_dirty,
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
{
    bool dirty = false;
    if ((_draw_to_texture != in_draw_to_texture) ||
        (_allow_clear != in_allow_clear) ||
        (_clear_colour != in_clear_colour))
    {
        _draw_to_texture = in_draw_to_texture;
        _allow_clear = in_allow_clear;
        _clear_colour = in_clear_colour;

        dirty = true;
        _has_drawn = false;
        _render_target_texture.reset();
    }
    if (_always_dirty != in_always_dirty)
    {
        dirty = true;
        _always_dirty = in_always_dirty;
    }

    return dirty;
}

void UITexture::SetSize(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorInt2& in_size
    )
{
    if (_size == in_size)
    {
        return;
    }

    _has_drawn = false;
    _size = in_size;
    if (nullptr != _render_target_texture)
    {
        in_draw_system->ResizeRenderTargetTexture(
            in_command_list,
            _render_target_texture.get(),
            _size
            );
    }

    return;
}

// nullptr if _bUseBackBuffer is true
std::shared_ptr<HeapWrapperItem> UITexture::GetShaderResourceHeapWrapperItem() const
{
    if ((true == _draw_to_texture) && (nullptr != _render_target_texture))
    {
        return _render_target_texture->GetShaderResourceHeapWrapperItem();
    }
    return nullptr;
}
