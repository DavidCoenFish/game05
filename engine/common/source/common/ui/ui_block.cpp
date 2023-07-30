#include "common/common_pch.h"
#include "common/ui/ui_block.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/math/vector_int2.h"

//std::shared_ptr<RenderTargetTexture> _render_target;

class UiBlockImplementation
{
public:
    UiBlockImplementation(
            DrawSystem* const in_draw_system,
            ID3D12GraphicsCommandList* const in_command_list,
            const VectorInt2& in_size
        )
    {
        const std::vector<RenderTargetFormatData> target_format_data_array({
            RenderTargetFormatData(DXGI_FORMAT_R8G8B8A8_UNORM)
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

    std::shared_ptr<HeapWrapperItem> GetTexture() const
    {
        return _render_target->GetShaderResourceHeapWrapperItem();
    }

private:
    std::shared_ptr<RenderTargetTexture> _render_target;

};

UiBlock::UiBlock(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorInt2& in_size
    )
{
    _implementation = std::make_unique<UiBlockImplementation>(
        in_draw_system,
        in_command_list,
        in_size
        );
}

UiBlock::~UiBlock()
{
    // Nop
}

std::shared_ptr<HeapWrapperItem> UiBlock::GetTexture() const
{
    return _implementation->GetTexture();
}
