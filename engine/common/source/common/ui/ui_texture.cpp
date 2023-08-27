#include "common/common_pch.h"
#include "common/ui/ui_texture.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/render_target/render_target_texture.h"

UITexture::UITexture(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const bool in_use_back_buffer,
    const bool in_always_dirty,
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
    : _use_back_buffer(in_use_back_buffer)
    , _allow_clear(in_allow_clear)
    , _always_dirty(in_always_dirty)
    , _has_drawn(false)
    , _draw_system(in_draw_system)
{
    UpdateUsingBackBuffer(
        in_use_back_buffer,
        in_draw_system,
        in_command_list,
        VectorInt2(8,8),
        in_allow_clear,
        in_clear_colour
        );
}

UITexture::~UITexture()
{
    // Nop
}

IRenderTarget* UITexture::GetRenderTarget() const
{
    if (false == _use_back_buffer)
    {
        return _render_target_texture.get();
    }
    return _draw_system->GetRenderTargetBackBuffer();
}

const VectorInt2 UITexture::GetSize() const
{
    return GetRenderTarget()->GetSize();
}

void UITexture::Update(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorInt2& in_size,
    const bool in_draw_to_texture,
    const bool in_always_dirty,
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
{
    UpdateUsingBackBuffer(
        false == in_draw_to_texture,
        in_draw_system,
        in_command_list,
        in_size,
        in_allow_clear,
        in_clear_colour
        );
    _always_dirty = in_always_dirty;

}

void UITexture::SetSize(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorInt2& in_size
    )
{
    if (_render_target_texture)
    {
        in_draw_system->ResizeRenderTargetTexture(
            in_command_list,
            _render_target_texture.get(),
            in_size
            );
    }
}

// nullptr if _bUseBackBuffer is true
std::shared_ptr<HeapWrapperItem> UITexture::GetShaderResourceHeapWrapperItem() const
{
    if ((false == _use_back_buffer) && (nullptr != _render_target_texture))
    {
        return _render_target_texture->GetShaderResourceHeapWrapperItem();
    }
    return nullptr;
}

void UITexture::UpdateUsingBackBuffer(
    const bool in_use_back_buffer,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorInt2& in_size,
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
{
    _use_back_buffer = in_use_back_buffer;
    _allow_clear = in_allow_clear;

    if (true == _use_back_buffer)
    {
        _render_target_texture.reset();
    }
    else
    {
        if (nullptr == _render_target_texture)
        {
            const std::vector<RenderTargetFormatData> target_format_data_array({
                RenderTargetFormatData(
                    DXGI_FORMAT_B8G8R8A8_UNORM, 
                    _allow_clear,
                    in_clear_colour
                    )
                });

            _render_target_texture = in_draw_system->MakeRenderTargetTexture(
                in_command_list,
                target_format_data_array,
                RenderTargetDepthData(),
                in_size // TODO: do we need a pending state? dont know what the size will be yet?
                );
        }
        else
        {
            in_draw_system->ResizeRenderTargetTexture(
                in_command_list,
                _render_target_texture.get(),
                in_size
                // Todo:
                //_allow_clear,
                //in_clear_colour
                );
        }
    }

    return;
}