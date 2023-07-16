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
        , _primitive_topology(in_primitive_topology)
        , _input_element_desc_array(in_input_element_desc_array)
        , _vertex_data(in_vertex_data)
        , _vertex_buffer()
        , _vertex_buffer_view()
    {
        // Nop
    }

    void Draw(ID3D12GraphicsCommandList* const in_command_list)
    {
        IGeometry::DrawImplementation(
            in_command_list,
            (int) _vertex_data.size(),
            _primitive_topology,
            _vertex_buffer_view
            );
    }


private:
    virtual void OnDeviceLost() override
    {
        IGeometry::DeviceLostImplementation(_vertex_buffer);
    }

    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device
        ) override
    {
        IGeometry::DeviceRestoredImplementation(
            _draw_system,
            in_command_list,
            in_device,
            (int) _vertex_data.size(),
            sizeof (TypeVertex),
            _vertex_buffer,
            _vertex_buffer_view,
            _vertex_data.in_data()
            );
    }


private:
    const D3D_PRIMITIVE_TOPOLOGY _primitive_topology;
    const std::vector < D3D12_INPUT_ELEMENT_DESC > _input_element_desc_array;
    std::vector < TypeVertex > _vertex_data;
    Microsoft::WRL::ComPtr < ID3D12Resource > _vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW _vertex_buffer_view;
};
