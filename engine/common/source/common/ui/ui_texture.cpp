#include "common/common_pch.h"
#include "common/ui/ui_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/log/log.h"

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

const bool UITexture::CalculateNeedsToDraw() const
{
    // if we are always dirty, or if we have not drawn
    return _always_dirty || (false == _has_drawn);
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
    }
    return;
}

const bool UITexture::SetRenderTarget(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame
    )
{
    LOG_MESSAGE_UI_VERBOSE("  UITexture::SetRenderTarget _draw_to_texture:%d _allow_clear:%d _always_dirty:%d _has_drawn:%d", _draw_to_texture, _allow_clear, _always_dirty, _has_drawn);
    LOG_MESSAGE_UI_VERBOSE("      _size:[%d, %d] _clear_colour[%f, %f, %f, %f]", _size[0], _size[1], _clear_colour[0], _clear_colour[1], _clear_colour[2], _clear_colour[3]);

    if (false == CalculateSizeValid(in_draw_system))
    {
        return false;
    }

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

const bool UITexture::CalculateSizeValid(
    DrawSystem* const in_draw_system
    ) const
{
    const VectorInt2 size = GetSize(in_draw_system);
    if ((size[0] <= 0) || 
        (size[1] <= 0))
    {
        return false;
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

const bool UITexture::SetSize(
    const VectorInt2& in_size
    )
{
    if (_size == in_size)
    {
        return false;
    }

    _size = in_size;
    _has_drawn = false;

    // resizing the render target seems to be triggering a object deleted while still in use error, so reset the resource on resize
    _render_target_texture.reset();

    return true;
}

const bool UITexture::SetShaderResource(
    Shader& in_shader,
    const int in_index,
    DrawSystemFrame* const in_draw_system_frame,
    DrawSystem* const in_draw_system
    )
{
    if (false == CalculateSizeValid(in_draw_system))
    {
        return false;
    }

    if ((true == _draw_to_texture) && (nullptr != _render_target_texture))
    {
        in_shader.SetShaderResourceViewHandle(
            in_index,
            _render_target_texture->GetShaderResourceHeapWrapperItem()
            );
        in_draw_system_frame->AddFrameResource(_render_target_texture);
    }

    return true;
}
