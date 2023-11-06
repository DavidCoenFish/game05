#include "common/common_pch.h"

#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/geometry/i_geometry.h"

GeometryGeneric::GeometryGeneric(
    DrawSystem* const in_draw_system,
    const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& in_input_element_desc_array,
    const std::vector<uint8_t>& in_vertex_data_raw,
    const int in_float_per_vertex
    ) 
    : IResource(in_draw_system)
    , _primitive_topology(in_primitive_topology)
    , _input_element_desc_array(in_input_element_desc_array)
    , _vertex_raw_data(in_vertex_data_raw)
    , _float_per_vertex(in_float_per_vertex)
    , _vertex_buffer()
    , _vertex_buffer_view{}
{
    // Nop
}

void GeometryGeneric::Draw(ID3D12GraphicsCommandList* const in_command_list)
{
    if (_vertex_buffer)
    {
        IGeometry::DrawImplementation(
            in_command_list,
            (UINT)(_vertex_raw_data.size() / (sizeof(float) * _float_per_vertex)),
            _primitive_topology,
            _vertex_buffer_view
            );
    }
}

void GeometryGeneric::UpdateVertexData(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device,
    const std::vector<uint8_t>& in_vertex_data_raw
    )
{
    _vertex_raw_data = in_vertex_data_raw;
    if (0 == _vertex_raw_data.size())
    {
        IGeometry::DeviceLostImplementation(_vertex_buffer);
        return;
    }

    if (nullptr == _vertex_buffer)
    {
        OnDeviceRestored(in_command_list, in_device);
    }
    else
    {
        const int byte_vertex_size = sizeof(float) * _float_per_vertex;
        IGeometry::UploadVertexData(
            in_draw_system,
            in_command_list,
            (int)(_vertex_raw_data.size() / byte_vertex_size),
            byte_vertex_size,
            _vertex_buffer,
            _vertex_buffer_view,
            _vertex_raw_data.data()
            );
    }
    return;
}

void GeometryGeneric::OnDeviceLost()
{
    IGeometry::DeviceLostImplementation(_vertex_buffer);
}

void GeometryGeneric::OnDeviceRestored(
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device
    )
{
    const int byte_vertex_size = sizeof(float) * _float_per_vertex;
    IGeometry::DeviceRestoredImplementation(
        _draw_system, //DrawSystem* const
        in_command_list, //ID3D12GraphicsCommandList* const 
        in_device, //ID3D12Device* const
        (int)(_vertex_raw_data.size() / byte_vertex_size), //const int in_vertex_count
        byte_vertex_size, //const int in_byte_vertex_size
        _vertex_buffer, //Microsoft::WRL::ComPtr < ID3D12Resource >& in_vertex_buffer
        _vertex_buffer_view, //D3D12_VERTEX_BUFFER_VIEW& in_vertex_buffer_view
        _vertex_raw_data.data() //void* in_raw_data
        );
}

