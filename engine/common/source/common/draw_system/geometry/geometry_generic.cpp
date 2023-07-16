#include "common/common_pch.h"

#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/geometry/i_geometry.h"

GeometryGeneric::GeometryGeneric(
    DrawSystem* const in_draw_system,
    const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
    const std::vector < D3D12_INPUT_ELEMENT_DESC >&in_input_element_desc_array,
    const std::vector < float >&in_vertex_data_raw,
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
    IGeometry::DrawImplementation(
        in_command_list,
        (UINT)(_vertex_raw_data.size() / _float_per_vertex),
        _primitive_topology,
        _vertex_buffer_view
        );
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
    IGeometry::DeviceRestoredImplementation(
        _draw_system,
        in_command_list,
        in_device,
        (int)(_vertex_raw_data.size() / _float_per_vertex),
        sizeof (float) * _float_per_vertex,
        _vertex_buffer,
        _vertex_buffer_view,
        _vertex_raw_data.data()
        );
}

