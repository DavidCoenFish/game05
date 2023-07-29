#include "common/common_pch.h"
#include "common/text/text_geometry.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/text/text_face.h"
#include "common/text/text_geometry.h"

namespace
{
    const std::vector<uint8_t> GenerateVertexData(
        //TextFace* const in_text_face,
        const std::string& in_string_utf8,
        const bool in_width_limit_enabled,
        const int in_width_limit,
        const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
        const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
        )
    {
        // TODO
        return std::vector<uint8_t>();
    }

}

TextGeometry::TextGeometry(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const std::string& in_string_utf8,
    const VectorInt2& in_containter_size,
    const bool in_width_limit_enabled,
    const int in_width_limit,
    const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment,
    const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
    )
: _string_utf8(in_string_utf8)
, _geometry()
, _container_size(in_containter_size)
, _width_limit_enabled(in_width_limit_enabled)
, _width_limit(in_width_limit)
, _horizontal_line_alignment(in_horizontal_line_alignment)
, _vertical_block_alignment(in_vertical_block_alignment)
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_element_desc_array(
        {
            D3D12_INPUT_ELEMENT_DESC
            {
                "POSITION",
                0,
                DXGI_FORMAT_R32G32_FLOAT,
                0,
                D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                0 // UINT InstanceDataStepRate;
            },
            D3D12_INPUT_ELEMENT_DESC
            {
                "TEXCOORD",
                0,
                DXGI_FORMAT_R32G32_FLOAT,
                0,
                D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                0 // UINT InstanceDataStepRate;
            },

            D3D12_INPUT_ELEMENT_DESC
            {
                "COLOR",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                0 // UINT InstanceDataStepRate;
            }
        }

    );

    const auto vertex_data_raw = GenerateVertexData(
        _string_utf8,
        _width_limit_enabled,
        _width_limit,
        _horizontal_line_alignment,
        _vertical_block_alignment
        );

    _geometry = in_draw_system->MakeGeometryGeneric(
        in_command_list,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
        input_element_desc_array,
        vertex_data_raw,
        8
        );
    return;
}

// return True if the value changes
const bool TextGeometry::SetText(
    const std::string& in_string_utf8
    )
{
    if (_string_utf8 == in_string_utf8)
    {
        return false;
    }
    _string_utf8 = in_string_utf8;
    return true;
}

// Get the natural size required by the text using current width limit if enabled
VectorInt2 TextGeometry::GetTextBounds() const
{
    return _text_bounds;
}

const bool TextGeometry::SetTextContainerSize(
    const VectorInt2& in_container_size
    )
{
    if (_container_size == in_container_size)
    {
        return false;
    }
    _container_size = in_container_size;
    return true;
}

VectorInt2 TextGeometry::GetTextContainerSize() const
{
    return _container_size;
}

const bool TextGeometry::SetWidthLimit(
    const bool in_width_limit_enabled,
    const int in_width_limit
    )
{
    if ((_width_limit_enabled == in_width_limit_enabled) &&
        (_width_limit == in_width_limit))
    {
        return false;
    }
    _width_limit_enabled = in_width_limit_enabled;
    _width_limit = in_width_limit;
    return true;
}

const bool TextGeometry::SetHorizontalLineAlignment(
    const TextEnum::HorizontalLineAlignment::Enum in_horizontal_line_alignment
    )
{
    if (_horizontal_line_alignment == in_horizontal_line_alignment)
    {
        return false;
    }
    _horizontal_line_alignment = in_horizontal_line_alignment;
    return true;
}

const bool TextGeometry::SetVerticalBlockAlignment(
    const TextEnum::VerticalBlockAlignment::Enum in_vertical_block_alignment
    )
{
    if (_vertical_block_alignment == in_vertical_block_alignment)
    {
        return false;
    }
    _vertical_block_alignment = in_vertical_block_alignment;
    return true;
}

void TextGeometry::RegenerateVertexData(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    const auto vertex_data_raw = GenerateVertexData(
        _string_utf8,
        _width_limit_enabled,
        _width_limit,
        _horizontal_line_alignment,
        _vertical_block_alignment
        );
    _geometry->UpdateVertexData(
        in_draw_system,
        in_command_list,
        vertex_data_raw
        );
}

GeometryGeneric* const TextGeometry::GetGeometry() const
{
    return _geometry.get();
}
