#include "common/common_pch.h"

#include "common/direct_xtk12/direct_x_helpers.h"
#include "common/direct_xtk12/graphics_memory.h"
#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/shader_resource.h"

ShaderResource::ShaderResource(
    DrawSystem* const in_draw_system,
    const std::shared_ptr<HeapWrapperItem>&in_shader_resource,
    const D3D12_RESOURCE_DESC& in_desc,
    const D3D12_SHADER_RESOURCE_VIEW_DESC& in_shader_resource_view_desc,
    const std::vector<uint8_t>& in_data
    ) 
    : IResource(in_draw_system)
    , _shader_resource(in_shader_resource)
    , _desc(in_desc)
    , _shader_resource_view_desc(in_shader_resource_view_desc)
    , _data(in_data)
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

void ShaderResource::UploadResource(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    Microsoft::WRL::ComPtr<ID3D12Resource> &in_resource,
    const D3D12_RESOURCE_DESC& in_desc,
    const size_t in_data_size,
    void* in_data
    )
{
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
        ShaderResource2DData.RowPitch = in_desc.Width * bytes_per_texel;
        ShaderResource2DData.SlicePitch = in_data_size;
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

    return;
}

void ShaderResource::OnDeviceRestored(
    ID3D12GraphicsCommandList* const in_command_list,
    ID3D12Device2* const in_device
    )
{
    {
        CD3DX12_HEAP_PROPERTIES heap_default(D3D12_HEAP_TYPE_DEFAULT);
        DX::ThrowIfFailed(in_device->CreateCommittedResource(
            &heap_default,
            D3D12_HEAP_FLAG_NONE,
            &_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf())
            ));
        _resource->SetName(L"Shader Texture2D resource");

        UploadResource(
            _draw_system,
            in_command_list,
            _resource,
            _desc,
            _data.size(),
            _data.size() ? _data.data() : nullptr
            );

        const int frame_count = _draw_system->GetBackBufferCount();
        for (int index = 0; index < frame_count; ++index)
        {
            in_device->CreateShaderResourceView(
                _resource.Get(),
                &_shader_resource_view_desc,
                _shader_resource->GetCPUHandleFrame(index)
            );
        }
        return;
    }

    return;
}

