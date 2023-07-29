#pragma once

#include "common/text/text_enum.h"

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class Shader;
class TextBlockImplementation;
class VectorInt2;

class TextBlock
{
public:
    TextBlock(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::string& in_string_utf8,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        );

    void SetText(
        const std::string& in_string_utf8
        );

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds() const;

    void SetTextContainerSize(
        const VectorInt2& in_size
        );

    void SetWidthLimit(
        const bool in_width_limit_enabled,
        const int in_width_limit
        );
    void SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment
        );
    void SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        );

    // Which is better, two pass, or one function that coould do two things
#if 1
    void Update(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        Shader* const in_shader
        );
    std::shared_ptr<HeapWrapperItem> GetTexture() const;
#else
    std::shared_ptr<HeapWrapperItem> UpdateGetTexture(
        ID3D12GraphicsCommandList* const in_command_list
        ) const;
#endif

private:
    std::unique_ptr<TextBlockImplementation> _implementation;

};
