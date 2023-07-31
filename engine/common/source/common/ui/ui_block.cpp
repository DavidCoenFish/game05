#include "common/common_pch.h"
#include "common/ui/ui_block.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

//std::shared_ptr<RenderTargetTexture> _render_target;

class UiBlockImplementation
{
public:
    UiBlockImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const VectorInt2& in_size,
        const bool in_clear_on_set,
        const VectorFloat4& in_clear_color
        )
    {
        const std::vector<RenderTargetFormatData> target_format_data_array({
            RenderTargetFormatData(
                DXGI_FORMAT_B8G8R8A8_UNORM,
                in_clear_on_set,
                in_clear_color
                )
            });

        _render_target = in_draw_system->MakeRenderTargetTexture(
            in_command_list,
            target_format_data_array,
            RenderTargetDepthData(),
            in_size.GetX(),
            in_size.GetY(),
            true
        );
    }

    std::shared_ptr<HeapWrapperItem> GetTexture(
        DrawSystemFrame* const in_frame
        )
    {
        in_frame->ResourceBarrier(
            _render_target.get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
        return _render_target->GetShaderResourceHeapWrapperItem();
    }

    void Activate(
        DrawSystemFrame* const in_frame
        )
    {
        in_frame->SetRenderTarget(_render_target.get());
    }

private:
    std::shared_ptr<RenderTargetTexture> _render_target;

};

UiBlock::UiBlock(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorInt2& in_size,
    const bool in_clear_on_set,
    const VectorFloat4& in_clear_color
    )
{
    _implementation = std::make_unique<UiBlockImplementation>(
        in_draw_system,
        in_command_list,
        in_size,
        in_clear_on_set,
        in_clear_color
        );
}

UiBlock::~UiBlock()
{
    // Nop
}

std::shared_ptr<HeapWrapperItem> UiBlock::GetTexture(
    DrawSystemFrame* const in_frame
    )
{
    return _implementation->GetTexture(in_frame);
}

void UiBlock::Activate(
    DrawSystemFrame* const in_frame
    )
{
    _implementation->Activate(in_frame);
    return;
}
