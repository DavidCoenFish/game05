#pragma once

#include "common/text/text_enum.h"
#include "common/math/vector_int2.h"

class DrawSystem;
class GeometryGeneric;

class TextGeometry
{
public:
    TextGeometry(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::string& in_string_utf8,
        const VectorInt2& in_containter_size,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment,
        const std::vector<uint8_t>& in_vertex_data_raw
        );

    // return True if the value changes
    const bool SetText(
        const std::string& in_string_utf8
        );
    const std::string GetTest() const
    {
        return _string_utf8;
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetTextBounds() const;

    const bool SetTextContainerSize(
        const VectorInt2& in_size
        );
    VectorInt2 GetTextContainerSize() const;

    const bool SetWidthLimit(
        const bool in_width_limit_enabled,
        const int in_width_limit
        );
    const bool GetWidthLimitEnabled() const
    {
        return _width_limit_enabled;
    }
    const int GetWidthLimit() const
    {
        return _width_limit;
    }


    const bool SetHorizontalLineAlignment(
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment
        );
    const TextEnum::HorizontalLineAlignment::Enum GetHorizontalLineAlignment() const
    {
        return _horizontal_line_alignment;
    }


    const bool SetVerticalBlockAlignment(
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        );
    const TextEnum::VerticalBlockAlignment::Enum GetVerticalBlockAlignment() const
    {
        return _vertical_block_alignment;
    }

    void UpdateVertexData(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::vector<uint8_t>& in_vertex_data_raw
        );

    GeometryGeneric* const GetGeometry() const;

private:
    std::string _string_utf8;
    std::shared_ptr<GeometryGeneric> _geometry;
    VectorInt2 _text_bounds;
    VectorInt2 _container_size;
    bool _width_limit_enabled;
    int _width_limit;
    TextEnum::HorizontalLineAlignment::Enum _horizontal_line_alignment;
    TextEnum::VerticalBlockAlignment::Enum _vertical_block_alignment;

};
