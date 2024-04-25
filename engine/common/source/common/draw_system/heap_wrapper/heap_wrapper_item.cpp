#include "common/common_pch.h"

#include "common/draw_system/heap_wrapper/heap_wrapper.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"

std::shared_ptr < HeapWrapperItem > HeapWrapperItem::Factory(
    ID3D12Device2* const in_device,
    const std::shared_ptr < HeapWrapper >&in_heap_wrapper,
    const int in_length
    )
{
    const int index = in_heap_wrapper->GetFreeIndex(
        in_device,
        in_length
        );
    return std::make_shared<HeapWrapperItem>(
        in_heap_wrapper,
        index,
        in_length
        );
}

HeapWrapperItem::HeapWrapperItem(
    const std::shared_ptr < HeapWrapper >&in_heap_wrapper,
    const int in_index,
    const int in_length
    ) 
    : _heap_wrapper(in_heap_wrapper)
    , _index(in_index)
    , _length(in_length)
{
    return;
}

HeapWrapperItem::~HeapWrapperItem()
{
    if (_heap_wrapper)
    {
        _heap_wrapper->FreeIndex(
            _index,
            _length
            );
    }
    return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetCPUHandle()
{
    if (_heap_wrapper)
    {
        return _heap_wrapper->GetCPUHandle(_index);
    }
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetCPUHandleFrame()
{
    if (_heap_wrapper)
    {
        return _heap_wrapper->GetCPUHandleFrame(
            _index
            );
    }
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapperItem::GetGPUHandle()
{
    if (_heap_wrapper)
    {
        return _heap_wrapper->GetGPUHandle(_index);
    }
    return D3D12_GPU_DESCRIPTOR_HANDLE();
}

ID3D12DescriptorHeap* const HeapWrapperItem::GetHeap()
{
    if (_heap_wrapper)
    {
        return _heap_wrapper->GetHeap(_index);
    }
    return nullptr;
}

