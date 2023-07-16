#include "common/common_pch.h"

#include "common/direct_xtk12/direct_x_helpers.h"
#include "common/direct_xtk12/graphics_memory.h"
#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/shader_resource.h"

ShaderResource::ShaderResource(
    DrawSystem* const in_draw_system,
    const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
    const D3D12_RESOURCE_DESC&in_desc,
    const D3D12_SHADER_RESOURCE_VIEW_DESC&in_shader_resource_view_desc,
    const std::vector < uint8_t >&in_data
    ) 
    : IResource(in_draw_system)
    , _shader_resource(in_shader_resource)
    , _desc(in_desc)
    , _shader_resource_view_desc(in_shader_resource_view_desc)
    , _data(in_data)// , m_bDataDirty(true)

{
    return;
}

void ShaderResource::OnDeviceLost()
{
    _resource.Reset();
}

std::shared_ptr < HeapWrapperItem > ShaderResource::GetHeapWrapperItem() const
{
    return _shader_resource;
}

void ShaderResource::UploadCreateResource(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device* const in_device,
    Microsoft::WRL::ComPtr < ID3D12Resource >&in_resource,
    std::shared_ptr < HeapWrapperItem >&in_shader_resource,
    const D3D12_RESOURCE_DESC&in_desc,
    const D3D12_SHADER_RESOURCE_VIEW_DESC&in_shader_resource_view_desc,
    const size_t in_data_size,
    void* in_data,
    LPCWSTR in_name
    )
{
    CD3DX12_HEAP_PROPERTIES heap_default(D3D12_HEAP_TYPE_DEFAULT);
    DX::ThrowIfFailed(in_device->CreateCommittedResource(
        &heap_default,
        D3D12_HEAP_FLAG_NONE,
        &in_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(in_resource.ReleaseAndGetAddressOf())
        ));
    in_resource->SetName(in_name);
    if (nullptr != in_data)
    {
        const UINT64 upload_buffer_size = GetRequiredIntermediateSize(
            in_resource.Get(),
            0,
            1
            );
        auto memory_upload = in_draw_system->AllocateUpload(
            upload_buffer_size,
            nullptr,
            D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT
            );
        const int bytes_per_texel = (int) DirectX::BytesPerPixel(in_desc.Format);
        // //D3D12_ShaderResource2D_DATA_PITCH_ALIGNMENT
        D3D12_SUBRESOURCE_DATA ShaderResource2DData = {};
        ShaderResource2DData.pData = in_data;
        ShaderResource2DData.RowPitch = in_desc.Width* bytes_per_texel, ShaderResource2DData.SlicePitch = in_data_size;
        if (in_command_list)
        {
            UpdateSubresources(
                in_command_list,
                in_resource.Get(),
                memory_upload.Resource(),
                memory_upload.ResourceOffset(),
                0,
                1,
                &ShaderResource2DData
                );
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                in_resource.Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
                );
            in_command_list->ResourceBarrier(
                1,
                &barrier
                );
        }
    }
    const int frame_count = in_draw_system->GetBackBufferCount();
    for (int index = 0; index < frame_count;++ index)
    {
        in_device->CreateShaderResourceView(
            in_resource.Get(),
            &in_shader_resource_view_desc,
            in_shader_resource->GetCPUHandleFrame(index)
            );
    }
    return;
}

// Std::vector<uint8_t>& ShaderResource::GetData()
// {
// M_bDataDirty = true;
// Return m_data;
// }
// Void ShaderResource::WriteData(
// Const int x,
// Const int y,
// Const size_t dataSizeBytes,
// Void* pData
// )
// {
// Const int bytesPerTexel = (int)DirectX::BytesPerPixel(m_desc.Format);
// Const int pixelCount = (int)dataSizeBytes / bytesPerTexel;
// If (false == m_bDataDirty)
// {
// M_bDataDirty = true;
// M_dataDirtyBox.left = x;
// M_dataDirtyBox.right = x + pixelCount;
// M_dataDirtyBox.top = y;
// M_dataDirtyBox.bottom = y + 1;
// M_dataDirtyBox.front = 0;
// M_dataDirtyBox.back = 1;
// }
// Else
// {
// M_dataDirtyBox.left = std::min<UINT>(x, m_dataDirtyBox.left);
// M_dataDirtyBox.right = std::max<UINT>(x + pixelCount, m_dataDirtyBox.right);
//
// M_dataDirtyBox.top = std::min<UINT>(y, m_dataDirtyBox.top);
// M_dataDirtyBox.bottom = std::max<UINT>(y + 1, m_dataDirtyBox.bottom);
// }
//
// Memcpy( &m_data[(y * m_desc.Width) + x], pData, dataSizeBytes);
//
// Return;
// }
// Void ShaderResource::UploadChangesIfNeeded(
// ID3D12GraphicsCommandList* const pCommandList
// )
// {
// If (false == m_bDataDirty)
// {
// Return;
// }
// M_bDataDirty = false;
//
\
    // PCommandList->ResourceBarrier(m_pResource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
//
// #if 0
// Const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(), 0, 1);
\
    // Auto pMemoryUpload = m_pDrawSystem->AllocateUpload(uploadBufferSize, nullptr, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
//
// Const int bytesPerTexel = (int)DirectX::BytesPerPixel(m_desc.Format);
//
// ////D3D12_ShaderResource2D_DATA_PITCH_ALIGNMENT
// D3D12_SUBRESOURCE_DATA ShaderResource2DData = {};
// ShaderResource2DData.pData = m_data.data();
// ShaderResource2DData.RowPitch = m_desc.Width * bytesPerTexel,
// ShaderResource2DData.SlicePitch = m_data.size();
//
// PCommandList->UpdateSubresourcesMethod(
// M_pResource.Get(),
// PMemoryUpload.Resource(),
// PMemoryUpload.ResourceOffset(),
// 0,
// 1,
// &ShaderResource2DData
// );
// #else
// Const UINT64 rowSizeBytes = GetRowSizeBytes(m_pResource.Get(), 0, 1);
// Const UINT rowCount = m_dataDirtyBox.bottom - m_dataDirtyBox.top;
\
    // Auto pMemoryUpload = m_pDrawSystem->AllocateUpload(rowSizeBytes * rowCount, nullptr, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
//
// Const int bytesPerTexel = (int)DirectX::BytesPerPixel(m_desc.Format);
//
// // take a window of the total data, keeping the same width as the source data
// D3D12_SUBRESOURCE_DATA ShaderResource2DData = {};
// ShaderResource2DData.pData = &m_data[m_dataDirtyBox.top * m_desc.Width];
// ShaderResource2DData.RowPitch = m_desc.Width * bytesPerTexel,
// ShaderResource2DData.SlicePitch = rowSizeBytes * rowCount;
//
// PCommandList->UpdateSubresourcesMethod(
// M_pResource.Get(),
// PMemoryUpload.Resource(),
// PMemoryUpload.ResourceOffset(),
// 0,
// 1,
// &ShaderResource2DData,
// &m_dataDirtyBox
// );
//
// #endif
//
\
    // PCommandList->ResourceBarrier(m_pResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
// }
void ShaderResource::OnDeviceRestored(
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device
    )
{
    UploadCreateResource(
        _draw_system,
        in_command_list,
        in_device,
        _resource,
        _shader_resource,
        _desc,
        _shader_resource_view_desc,
        _data.size(),
        _data.size() ? _data.data() : nullptr,
        L"Shader Texture2D resource"
        );
    // M_bDataDirty = false;
    return;
}

