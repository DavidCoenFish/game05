#pragma once
#include "common/draw_system/geometry/i_geometry.h"
#include "common/draw_system/i_resource.h"

template < typename TypeVertex > class Geometry : public IResource
{
public:
    Geometry(
        DrawSystem* const in_draw_system,
        const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
        const std::vector < D3D12_INPUT_ELEMENT_DESC >&in_input_element_desc_array,
        const std::vector < TypeVertex >&in_vertex_data
        ) 
        : IResource(in_draw_system)
        , primitive_topology(in_primitive_topology)
        , input_element_desc_array(in_input_element_desc_array)
        , vertex_data(in_vertex_data)
        , vertex_buffer()
        , vertex_buffer_view()
    {
        // Nop
    }

    void Draw(ID3D12GraphicsCommandList* const in_command_list)
    {
        IGeometry::DrawImplementation(
            in_command_list,
            (int) in_vertex_data.in_size(),
            in_primitive_topology,
            in_vertex_buffer_view
            );
    }


private:
    virtual void OnDeviceLost() override
    {
        IGeometry::DeviceLostImplementation(in_vertex_buffer);
    }

    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device
        ) override
    {
        IGeometry::DeviceRestoredImplementation(
            in_draw_system,
            in_command_list,
            in_device,
            (int) in_vertex_data.in_size(),
            sizeof (TypeVertex),
            in_vertex_buffer,
            in_vertex_buffer_view,
            in_vertex_data.in_data()
            );
    }


private:
    const D3D_PRIMITIVE_TOPOLOGY primitive_topology;
    const std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
    std::vector < TypeVertex > vertex_data;
    Microsoft::WRL::ComPtr < ID3D12Resource > vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
};
