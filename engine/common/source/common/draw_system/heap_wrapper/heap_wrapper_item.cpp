#include "common/common_pch.h"

#include "common/draw_system/heap_wrapper/heap_wrapper.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"

std::shared_ptr < HeapWrapperItem > HeapWrapperItem::Factory(
    ID3D12Device2* const in_device,
    const std::shared_ptr < HeapWrapper >&in_heap_wrapper,
    const int in_length
    )
{
    const int in_index = in_heap_wrapper->GetFreeIndex(
        in_device,
        in_length
        );
    return std::make_shared < HeapWrapperItem > (
        in_heap_wrapper,
        in_index,
        in_length
        );
}

HeapWrapperItem::HeapWrapperItem(
    const std::shared_ptr < HeapWrapper >&in_heap_wrapper,
    const int in_index,
    const int in_length
    ) 
    : heap_wrapper(in_heap_wrapper)
    , index(in_index)
    , length(in_length)
{
    return;
}

HeapWrapperItem::~HeapWrapperItem()
{
    if (heap_wrapper)
    {
        heap_wrapper->FreeIndex(
            in_index,
            in_length
            );
    }
    return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetCPUHandle()
{
    if (heap_wrapper)
    {
        return heap_wrapper->GetCPUHandle(in_index);
    }
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetCPUHandleFrame(const int in_frame_index)
{
    if (heap_wrapper)
    {
        return heap_wrapper->GetCPUHandleFrame(
            in_index,
            in_frame_index
            );
    }
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetGPUHandle()
{
    if (heap_wrapper)
    {
        return heap_wrapper->GetGPUHandle(in_index);
    }
    return D3D12_GPU_DESCRIPTOR_HANDLE();
}

ID3D12DescriptorHeap* const HeapWrapperItem::GetHeap()
{
    if (heap_wrapper)
    {
        return heap_wrapper->GetHeap(in_index);
    }
    return nullptr;
}

