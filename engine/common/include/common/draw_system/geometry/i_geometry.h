#pragma once
// #include "Common/DrawSystem/IResource.h"
class DrawSystem;

class IGeometry // : public IResource

{
public:
    // IGeometry(DrawSystem* const pDrawSystem);
    // Virtual ~IGeometry();
    // Virtual void Draw(ID3D12GraphicsCommandList* const pCommandList) = 0;
    // Protected:
    static void DrawImplementation(
        ID3D12GraphicsCommandList* const in_command_list,
        const UINT in_vertex_count,
        const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
        D3D12_VERTEX_BUFFER_VIEW&in_vertex_buffer_view
        );
    static void DeviceLostImplementation(Microsoft::WRL::ComPtr < ID3D12Resource >&in_vertex_buffer);
    static void DeviceRestoredImplementation(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device* const in_device,
        const int in_vertex_count,
        const int in_byte_vertex_size,
        Microsoft::WRL::ComPtr<ID3D12Resource>& in_vertex_buffer,
        D3D12_VERTEX_BUFFER_VIEW& in_vertex_buffer_view,
        void* in_raw_data
        );

    static void UploadVertexData(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const int in_vertex_count,
        const int in_byte_vertex_size,
        Microsoft::WRL::ComPtr<ID3D12Resource>& in_vertex_buffer,
        void* in_raw_data
        );

};
