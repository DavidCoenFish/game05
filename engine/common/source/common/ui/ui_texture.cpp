#include "common/common_pch.h"
#include "common/ui/ui_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
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
    //, _needs_resize(false)
    , _clear_colour(in_clear_colour)
{
    // Nop
}

UITexture::~UITexture()
{
    _render_target_texture.reset();
}

void UITexture::UpdateRenderTarget(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    if ((true == _draw_to_texture) && (0 < _size.GetX()) && (0 < _size.GetY()))
    {
        if (nullptr == _render_target_texture)
        {
            _has_drawn = false;
            //_needs_resize = false;

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
#if 0
        else if (true == _needs_resize)
        {
            _has_drawn = false;
            _needs_resize = false;

            in_draw_system->ResizeRenderTargetTexture(
                in_command_list,
                _render_target_texture.get(),
                _size
                );
        }
#endif
    }
    return;
}

const bool UITexture::SetRenderTarget(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame
    )
{
    UpdateRenderTarget(in_draw_system, in_frame->GetCommandList());

    if (true == _draw_to_texture)
    {
        if (nullptr == _render_target_texture)
        {
            return false;
        }

        in_frame->SetRenderTarget(
            _render_target_texture.get(),
            _allow_clear
            );
        in_frame->AddFrameResource(_render_target_texture);
    }
    else
    {
        in_frame->SetRenderTarget(
            in_draw_system->GetRenderTargetBackBuffer(),
            _allow_clear
            );
    }

    return true;
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
    const VectorInt2& in_size
    )
{
    if (_size == in_size)
    {
        return;
    }

    _size = in_size;
    _has_drawn = false;

#if 1
    _render_target_texture.reset();
#else
    // resizing the render target seems to be triggering a object deleted while still in use error
    _needs_resize = true;

    if ((_size.GetX() <= 0) ||
       (_size.GetY() <= 0))
    {
        _render_target_texture.reset();
    }
#endif

    return;
}

void UITexture::SetShaderResource(
    Shader& in_shader,
    const int in_index,
    DrawSystemFrame* const in_draw_system_frame
    )
{
    if ((true == _draw_to_texture) && (nullptr != _render_target_texture))
    {
        in_shader.SetShaderResourceViewHandle(
            in_index,
            _render_target_texture->GetShaderResourceHeapWrapperItem()
            );
        in_draw_system_frame->AddFrameResource(_render_target_texture);
    }

    return;
}
