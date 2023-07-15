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
    , primitive_topology(in_primitive_topology)
    , input_element_desc_array(in_input_element_desc_array)
    , vertex_raw_data(in_vertex_data_raw)
    , float_per_vertex(in_float_per_vertex)
    , vertex_buffer()
    , vertex_buffer_view{}
{
    // Nop
}

void GeometryGeneric::Draw(ID3D12GraphicsCommandList* const in_command_list)
{
    IGeometry::DrawImplementation(
        in_command_list,
        (UINT)(vertex_raw_data.size() / float_per_vertex),
        in_primitive_topology,
        in_vertex_buffer_view
        );
}

void GeometryGeneric::OnDeviceLost()
{
    IGeometry::DeviceLostImplementation(in_vertex_buffer);
}

void GeometryGeneric::OnDeviceRestored(
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device
    )
{
    IGeometry::DeviceRestoredImplementation(
        in_draw_system,
        in_command_list,
        in_device,
        (int)(vertex_raw_data.size() / float_per_vertex),
        sizeof (float) * float_per_vertex,
        in_vertex_buffer,
        in_vertex_buffer_view,
        vertex_raw_data.in_data()
        );
}

