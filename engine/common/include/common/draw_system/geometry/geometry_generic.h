#pragma once
#include "common/draw_system/i_resource.h"

class GeometryGeneric : public IResource
{
public:
    GeometryGeneric(
        DrawSystem* const in_draw_system,
        const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
        const std::vector < D3D12_INPUT_ELEMENT_DESC >& in_input_element_desc_array,
        const std::vector < float >& in_vertex_data_raw,
        const int in_float_per_vertex
        );
    void Draw(ID3D12GraphicsCommandList* const in_command_list);

private:
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device
        ) override;

private:
    const D3D_PRIMITIVE_TOPOLOGY _primitive_topology;
    const std::vector < D3D12_INPUT_ELEMENT_DESC > _input_element_desc_array;
    std::vector < float > _vertex_raw_data;
    int _float_per_vertex;
    Microsoft::WRL::ComPtr < ID3D12Resource > _vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW _vertex_buffer_view;
};