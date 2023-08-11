#include "common/common_pch.h"

#include "common/direct_xtk12/d3dx12.h"
#include "common/direct_xtk12/graphics_memory.h"
#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/geometry/i_geometry.h"

void IGeometry::DrawImplementation(
    ID3D12GraphicsCommandList* const in_command_list,
    const UINT in_vertex_count,
    const D3D_PRIMITIVE_TOPOLOGY in_primitive_topology,
    D3D12_VERTEX_BUFFER_VIEW& in_vertex_buffer_view
    )
{
    PIXBeginEvent(
        in_command_list,
        PIX_COLOR_DEFAULT,
        L"Draw"
        );
    in_command_list->IASetPrimitiveTopology(in_primitive_topology);
    in_command_list->IASetVertexBuffers(
        0,
        1,
        &in_vertex_buffer_view
        );
    in_command_list->DrawInstanced(
        in_vertex_count,
        1,
        0,
        0
        );
    PIXEndEvent(in_command_list);
}

void IGeometry::DeviceLostImplementation(Microsoft::WRL::ComPtr < ID3D12Resource >&in_vertex_buffer)
{
    in_vertex_buffer.Reset();
}

void IGeometry::DeviceRestoredImplementation(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device* const in_device,
    const int in_vertex_count,
    const int in_byte_vertex_size,
    Microsoft::WRL::ComPtr<ID3D12Resource>& in_vertex_buffer,
    D3D12_VERTEX_BUFFER_VIEW& in_vertex_buffer_view,
    void* in_raw_data
    )
{
    const int byte_total_size = in_byte_vertex_size* in_vertex_count;
    // We could return null vertex buffer on no geometry, but what then happens on an update
    if (0 == byte_total_size)
    {
        return;
    }
    auto buffer_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_total_size);

    {
        auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        in_device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &buffer_resource_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(in_vertex_buffer.ReleaseAndGetAddressOf())
            );
        in_vertex_buffer->SetName(L"GeometryVertexBuffer");
    }

    {
        if (in_command_list)
        {
            auto upload_memory = in_draw_system->AllocateUpload(byte_total_size);
            D3D12_SUBRESOURCE_DATA vertex_data = {};
            vertex_data.pData = in_raw_data;
            vertex_data.RowPitch = byte_total_size;
            vertex_data.SlicePitch = byte_total_size;
            UpdateSubresources(
                in_command_list,
                in_vertex_buffer.Get(),
                upload_memory.Resource(),
                upload_memory.ResourceOffset(),
                0,
                1,
                &vertex_data
                );
        }
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            in_vertex_buffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
            );
        in_command_list->ResourceBarrier(
            1,
            &barrier
            );
    }
    in_vertex_buffer_view.BufferLocation = in_vertex_buffer->GetGPUVirtualAddress();
    in_vertex_buffer_view.StrideInBytes = in_byte_vertex_size;
    in_vertex_buffer_view.SizeInBytes = byte_total_size;
    return;
}

void IGeometry::UploadVertexData(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const int in_vertex_count,
    const int in_byte_vertex_size,
    Microsoft::WRL::ComPtr<ID3D12Resource>& in_vertex_buffer,
    void* in_raw_data
    )
{
    if (nullptr == in_command_list)
    {
        return;
    }

    const int byte_total_size = in_byte_vertex_size * in_vertex_count;
    auto buffer_resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_total_size);
    auto upload_memory = in_draw_system->AllocateUpload(byte_total_size);

    {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            in_vertex_buffer.Get(),
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            D3D12_RESOURCE_STATE_COPY_DEST
        );
        in_command_list->ResourceBarrier(
            1,
            &barrier
        );
    }

    D3D12_SUBRESOURCE_DATA vertex_data = {};
    vertex_data.pData = in_raw_data;
    vertex_data.RowPitch = byte_total_size;
    vertex_data.SlicePitch = byte_total_size;
    UpdateSubresources(
        in_command_list,
        in_vertex_buffer.Get(),
        upload_memory.Resource(),
        upload_memory.ResourceOffset(),
        0,
        1,
        &vertex_data
    );

    {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            in_vertex_buffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
        );
        in_command_list->ResourceBarrier(
            1,
            &barrier
        );
    }

    return;
}
