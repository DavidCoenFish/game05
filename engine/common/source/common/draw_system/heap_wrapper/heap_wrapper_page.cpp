#include "common/common_pch.h"

#include "common/draw_system/d3dx12.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_page.h"

std::shared_ptr < HeapWrapperPage > HeapWrapperPage::Factory(
    const int in_frame_count,
    const D3D12_DESCRIPTOR_HEAP_DESC&in_desc,
    ID3D12Device2* const in_device
    )
{
    auto result = std::make_shared < HeapWrapperPage > (
        in_frame_count,
        in_desc.NumDescriptors
        );
    result->OnDeviceRestored(
        in_desc,
        in_device
        );
    return result;
}

HeapWrapperPage::HeapWrapperPage(
    const int in_frame_count,
    const int in_num_descriptors // Const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pDescriptorHeap,
    // Const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHeapStart,
    // Const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHeapStart

    ) 
    : frame_count(in_frame_count)
    , descriptor_heap()// PDescriptorHeap)

    , handle_increment_size(0)
    , cpu_heap_start()// CPUHeapStart)

    , gpu_heap_start()// GPUHeapStart)

    , index_high_water(0)
    , free_index()
    , num_descriptors(in_num_descriptors)
{
    return;
}

void HeapWrapperPage::OnDeviceLost()
{
    for (int index = 0; index < frame_count;++ index)
    {
        descriptor_heap[index].Reset();
    }
}

void HeapWrapperPage::OnDeviceRestored(
    const D3D12_DESCRIPTOR_HEAP_DESC&in_desc,
    ID3D12Device2* const in_device
    )
{
    if (nullptr == in_device)
    {
        return;
    }
    for (int index = 0; index < frame_count;++ index)
    {
        if (nullptr != descriptor_heap[index])
        {
            return;
        }
        DX::ThrowIfFailed(in_device->CreateDescriptorHeap(
            &in_desc,
            IID_PPV_ARGS(descriptor_heap[index].ReleaseAndGetAddressOf())
            ));
        cpu_heap_start[index] = descriptor_heap[index]->GetCPUDescriptorHandleForHeapStart();
        gpu_heap_start[index] = descriptor_heap[index]->GetGPUDescriptorHandleForHeapStart();
    }
    handle_increment_size = in_device->GetDescriptorHandleIncrementSize(in_desc.Type);
}

const int HeapWrapperPage::GetFreeIndex(const int in_length)
{
    if ((1 == in_length) && (0 < free_index.size()))
    {
        int index = free_index.back();
        free_index.pop_back();
        return index;
    }
    if (index_high_water + in_length <= num_descriptors)
    {
        int result = index_high_water;
        index_high_water += in_length;
        return result;
    }
    return - 1;
}

void HeapWrapperPage::FreeIndex(
    const int in_trace,
    const int in_length
    )
{
    for (int index = 0; index < in_length;++ index)
    {
        free_index.push_back(in_trace + index);
    }
    if (num_descriptors == free_index.size())
    {
        index_high_water = 0;
        free_index.clear();
    }
    return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperPage::GetCPUHandle(
    const int in_index,
    const int in_frame_index
    )
{
    const int local_frame_index = in_frame_index % frame_count;
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        in_cpu_heap_start[local_frame_index],
        in_index,
        in_handle_increment_size
        );
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapperPage::GetGPUHandle(
    const int in_index,
    const int in_frame_index
    )
{
    const int local_frame_index = in_frame_index % frame_count;
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(
        in_gpu_heap_start[local_frame_index],
        in_index,
        in_handle_increment_size
        );
}

ID3D12DescriptorHeap* const HeapWrapperPage::GetHeap(const int in_frame_index)
{
    const int local_frame_index = in_frame_index % frame_count;
    return descriptor_heap[local_frame_index].Get();
}

