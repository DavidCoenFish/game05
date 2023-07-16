#include "common/common_pch.h"

#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/draw_system/shader/constant_buffer.h"

ConstantBuffer::ConstantBuffer(
    const int in_frame_count,
    const size_t in_constant_buffer_size,
    const std::shared_ptr < HeapWrapperItem >&in_heap_wrapper_item,
    const void* const in_data,
    const D3D12_SHADER_VISIBILITY in_visiblity
    ) 
    : _frame_count(in_frame_count)
    , _constant_buffer_size(in_constant_buffer_size)
    , _heap_wrapper_item(in_heap_wrapper_item)
    , _data(in_data)
    , _visiblity(in_visiblity)
{
    for (int i = 0; i < MAX_BACK_BUFFER_COUNT;++ i)
    {
        _gpu_address[i] = 0;
    }
}

void ConstantBuffer::DeviceLost()
{
    for (int i = 0; i < _frame_count;++ i)
    {
        _constant_buffer_upload_heap[i].Reset();
        _gpu_address[i] = 0;
    }
    return;
}

void ConstantBuffer::DeviceRestored(ID3D12Device* const in_device)
{
    for (int i = 0; i < _frame_count;++ i)
    {
        const auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(1024* 64);
        DX::ThrowIfFailed(in_device->CreateCommittedResource(
            &heap_properties,
            // This heap will be used to upload the constant buffer data
            D3D12_HEAP_FLAG_NONE,
            // No flags
            &resource_desc,
            // Size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
            D3D12_RESOURCE_STATE_GENERIC_READ,
            // Will be data that is read from so we keep it in the generic read state
            nullptr,
            // We do not have use an optimized clear value for constant buffers
            IID_PPV_ARGS(&_constant_buffer_upload_heap[i])
            ));
        _constant_buffer_upload_heap[i]->SetName(L"Constant Buffer Upload Resource Heap");
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
        cbv_desc.BufferLocation = _constant_buffer_upload_heap[i]->GetGPUVirtualAddress();
        cbv_desc.SizeInBytes = (_constant_buffer_size + 255) &~255;
        // CB size is required to be 256-byte aligned.
        in_device->CreateConstantBufferView(
            &cbv_desc,
            _heap_wrapper_item->GetCPUHandleFrame(i)
            );
        // MainDescriptorHeap[i]->GetCPUDescriptorHandleForHeapStart());
        // ZeroMemory(&cbColorMultiplierData, sizeof(cbColorMultiplierData));
        CD3DX12_RANGE read_range(
            0,
            0
            );
        // We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
        DX::ThrowIfFailed(_constant_buffer_upload_heap[i]->Map(
            0,
            &read_range,
            reinterpret_cast < void** > (&_gpu_address[i])
            ));
        // Memcpy(m_pGPUAddress[i], &cbColorMultiplierData, sizeof(cbColorMultiplierData));
    }
}

void ConstantBuffer::Activate(
    ID3D12GraphicsCommandList* const in_command_list,
    // Void* const pData,
    const int in_root_param_index,
    const int in_frame_index
    )
{
    ID3D12DescriptorHeap* descriptor_heaps[] =
    {
        _heap_wrapper_item->GetHeap()
    };
    in_command_list->SetDescriptorHeaps(
        _countof(descriptor_heaps),
        descriptor_heaps
        );
    in_command_list->SetGraphicsRootDescriptorTable(
        in_root_param_index,
        _heap_wrapper_item->GetGPUHandle()
        );
    if (0 == _gpu_address[in_frame_index])
    {
        return;
    }
    memcpy(
        _gpu_address[in_frame_index],
        _data,
        _constant_buffer_size
        );
    return;
}

const int ConstantBuffer::GetNum32BitValues() const
{
    return (int)(_constant_buffer_size / 4);
}

