#include "common/common_pch.h"
#include "common/text/text_block.h"
#include "common/text/text_geometry.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float4.h"

class TextBlockImplementation
{
public:
    TextBlockImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::string& in_string_utf8,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        )
        : _geometry_dirty(false)
        //, _render_target_dirty(false)
    {
        //const std::vector<RenderTargetFormatData> target_format_data_array({
        //    RenderTargetFormatData(DXGI_FORMAT_R8G8B8A8_UNORM)
        //    });

        //_render_target = in_draw_system->MakeRenderTargetTexture(
        //    in_command_list,
        //    target_format_data_array,
        //    RenderTargetDepthData(),
        //    in_containter_size.GetX(),
        //    in_containter_size.GetY(),
        //    true
        //);

        std::vector<uint8_t> vertex_raw_data;
        _geometry = std::make_unique<TextGeometry>(
            in_draw_system,
            in_command_list,
            in_string_utf8,
            in_containter_size,
            in_width_limit_enabled,
            in_width_limit,
            in_horizontal_line_alignment,
            in_vertical_block_alignment,
            vertex_raw_data
            );
    }

    void SetText(
        const std::string& in_string_utf8
        )
    {
        if (true == _geometry->SetText(in_string_utf8))
        {
            _geometry_dirty = true;
        }
        return;
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds() const
    {
        return _geometry->GetTextBounds();
    }

    void SetTextContainerSize(
        const VectorInt2& in_size
        )
    {
        if (true == _geometry->SetTextContainerSize(in_size))
        {
            //_render_target_dirty = true;
            _geometry_dirty = true;
        }
        return;
    }

    void SetWidthLimit(
        const bool in_width_limit_enabled,
        const int in_width_limit
        )
    {
        if (true == _geometry->SetWidthLimit(in_width_limit_enabled, in_width_limit))
        {
            _geometry_dirty = true;
        }
        return;
    }

    void SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment
        )
    {
        if (true == _geometry->SetHorizontalLineAlignment(in_horizontal_line_alignment))
        {
            _geometry_dirty = true;
        }
        return;
    }

    void SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        )
    {
        if (true == _geometry->SetVerticalBlockAlignment(in_vertical_block_alignment))
        {
            _geometry_dirty = true;
        }
        return;
    }

    // Which is better, two pass, or one function that coould do two things
    void Update(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        Shader* const in_shader
    )
    {
#if 1
        std::vector<uint8_t> vertex_raw_data;
        _geometry->UpdateVertexData(
            in_draw_system,
            in_command_list,
            vertex_raw_data
        );
        return;
#else
        bool needs_to_draw = false;
        if (true == _render_target_dirty)
        {
            needs_to_draw = true;
            _render_target_dirty = false;
            const auto container_size = _geometry->GetTextContainerSize();
            in_draw_system->ResizeRenderTargetTexture(
                in_draw_system_frame->GetCommandList(),
                _render_target,
                container_size.GetX(),
                container_size.GetY()
                );
        }
        if (true == _geometry_dirty)
        {
            std::vector<uint8_t> vertex_raw_data;
            needs_to_draw = true;
            _geometry_dirty = false;
            _geometry->UpdateVertexData(
                in_draw_system,
                in_draw_system_frame->GetCommandList(),
                vertex_raw_data
                );
        }
        in_draw_system_frame->SetRenderTarget(_render_target.get());
        in_draw_system_frame->SetShader(in_shader);
        in_draw_system_frame->Draw(_geometry->GetGeometry());
        return;
#endif
    }

    //std::shared_ptr<HeapWrapperItem> GetTexture() const
    //{
    //    auto render_target = _render_target.get();
    //    if (nullptr != render_target)
    //    {
    //        return _render_target->GetShaderResourceHeapWrapperItem();
    //    }
    //    return nullptr;
    //}

private:
    // Todo, remove _render_target from here, use ui_block to host render targets?
    //std::shared_ptr<RenderTargetTexture> _render_target;
    std::unique_ptr<TextGeometry> _geometry;
    //bool _render_target_dirty;
    bool _geometry_dirty;

};

TextBlock::TextBlock(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::string& in_string_utf8,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
    )
{
    _implementation = std::make_unique<TextBlockImplementation>(
        in_draw_system,
        in_command_list,
        in_string_utf8,
        in_containter_size,
        in_width_limit_enabled,
        in_width_limit,
        in_horizontal_line_alignment,
        in_vertical_block_alignment
        );
}

TextBlock::~TextBlock()
{
    // Nop
}

void TextBlock::SetText(
    const std::string& in_string_utf8
    )
{
    _implementation->SetText(in_string_utf8);
    return;
}

// Get the natural size required by the text using current width limit if enabled
VectorInt2 TextBlock::GetTextBounds() const
{
    return _implementation->GetTextBounds();
}

void TextBlock::SetTextContainerSize(
    const VectorInt2& in_size
    )
{
    _implementation->SetTextContainerSize(in_size);
    return;
}

void TextBlock::SetWidthLimit(
    const bool in_width_limit_enabled,
    const int in_width_limit
    )
{
    _implementation->SetWidthLimit(
        in_width_limit_enabled,
        in_width_limit
        );
    return;
}

void TextBlock::SetHorizontalLineAlignment(
    const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment
    )
{
    _implementation->SetHorizontalLineAlignment(
        in_horizontal_line_alignment
        );
    return;
}

void TextBlock::SetVerticalBlockAlignment(
    const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
    )
{
    _implementation->SetVerticalBlockAlignment(
        in_vertical_block_alignment
        );
    return;
}

void TextBlock::Update(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    //DrawSystemFrame* const in_draw_system_frame,
    Shader* const in_shader
    )
{
    _implementation->Update(
        in_draw_system, 
        in_command_list,
        in_shader
        );
    return;
}

//std::shared_ptr<HeapWrapperItem> TextBlock::GetTexture() const
//{
//    return _implementation->GetTexture();
//}
//
