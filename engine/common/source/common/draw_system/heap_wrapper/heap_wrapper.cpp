#include "common/common_pch.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/heap_wrapper/heap_wrapper.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_page.h"

HeapWrapper::HeapWrapper(
    DrawSystem* const in_draw_system,
    const D3D12_DESCRIPTOR_HEAP_TYPE in_type,
    const int in_frame_count,
    const bool in_shader_visible,
    const UINT in_num_descriptors
    ) 
    : IResource(in_draw_system)
    , frame_count(in_frame_count)
    , desc()
{
    desc.Type = in_type;
    desc.NumDescriptors = in_num_descriptors;
    desc.Flags = (in_shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapper::GetCPUHandle(const int in_index)
{
    const int frame_index = draw_system ? draw_system->GetBackBufferIndex() : 0;
    return GetCPUHandleFrame(
        in_index,
        frame_index
        );
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapper::GetCPUHandleFrame(
    const int in_index,
    const int in_frame_index
    )
{
    const int page_index = in_index / desc.NumDescriptors;
    if ((0 <= page_index) && (page_index < (int) array_page.size()))
    {
        const int local_index = in_index - (desc.NumDescriptors* page_index);
        return array_page[page_index]->GetCPUHandle(
            local_index,
            in_frame_index
            );
    }
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapper::GetGPUHandle(const int in_index)
{
    assert(in_desc.Flags&D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    const int frame_index = draw_system ? draw_system->GetBackBufferIndex() : 0;
    const int page_index = in_index / in_desc.NumDescriptors;
    if ((0 <= page_index) && (page_index < (int) array_page.size()))
    {
        const int local_index = in_index - (in_desc.NumDescriptors* page_index);
        return array_page[page_index]->GetGPUHandle(
            local_index,
            frame_index
            );
    }
    return D3D12_GPU_DESCRIPTOR_HANDLE();
}

ID3D12DescriptorHeap* const HeapWrapper::GetHeap(const int in_index)
{
    const int frame_index = draw_system ? draw_system->GetBackBufferIndex() : 0;
    const int page_index = in_index / desc.NumDescriptors;
    if ((0 <= page_index) && (page_index < (int) array_page.size()))
    {
        return array_page[page_index]->GetHeap(frame_index);
    }
    return nullptr;
}

const int HeapWrapper::GetFreeIndex(
    ID3D12Device2* const in_device,
    const int in_length
    )
{
    int trace = 0;
    for (auto iter : array_page)
    {
        const int result = iter->GetFreeIndex(in_length);
        if (- 1 != result)
        {
            return result + trace;
        }
        trace += desc.NumDescriptors;
    }

    {
        auto page = HeapWrapperPage::Factory(
            in_frame_count,
            desc,
            in_device
            );
        const int result = page->GetFreeIndex(in_length);
        array_page.push_back(page);
        return result + trace;
    }
}

void HeapWrapper::FreeIndex(
    const int in_index,
    const int in_length
    )
{
    const int page_index = in_index / desc.NumDescriptors;
    if ((0 <= page_index) && (page_index < (int) array_page.size()))
    {
        const int local_index = in_index - (desc.NumDescriptors* page_index);
        array_page[page_index]->FreeIndex(
            local_index,
            in_length
            );
    }
    return;
}

void HeapWrapper::OnDeviceLost()
{
    for (auto iter : array_page)
    {
        iter->OnDeviceLost();
    }
    return;
}

void HeapWrapper::OnDeviceRestored(
    ID3D12GraphicsCommandList* const,
    ID3D12Device2* const in_device
    )
{
    for (auto iter : array_page)
    {
        iter->OnDeviceRestored(
            in_desc,
            in_device
            );
    }
    return;
}

