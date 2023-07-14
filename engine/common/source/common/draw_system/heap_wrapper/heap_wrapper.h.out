#pragma once
#include "common/draw_system/i_resource.h"

// Struct HeapWrapperIndex
// {
// Std::shared_ptr<HeapWrapper> m_pHeapWrapper;
// Int m_index;
// Int m_length;
// };
// Static
class HeapWrapperPage;
class DrawSystem;

// Https://docs.microsoft.com/en-us/windows/win32/direct3d12/creating-descriptor-heaps#descriptor-heap-types
class HeapWrapper : public IResource
{
public:
    HeapWrapper(
        DrawSystem* const in_draw_system,
        const D3D12_DESCRIPTOR_HEAP_TYPE in_type,
        const int in_frame_count = 1,
        const bool in_shader_visible = false,
        const UINT in_num_descriptors = 256
        );
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const int in_index);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleFrame(
        const int in_index,
        const int in_frame_index
        );
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const int in_index);
    ID3D12DescriptorHeap* const GetHeap(const int in_index);
    const int GetFreeIndex(
        ID3D12Device2* const in_device,
        const int in_length = 1
        );
    void FreeIndex(
        const int in_index,
        const int in_length = 1
        );

private:
    virtual void OnDeviceLost() _override;
    virtual void OnDeviceRestored(
        ID3D12GraphicsCommandList* const in_command_list,
        ID3D12Device2* const in_device
        ) _override;

private:
    int _frame_count;
    std::_vector < std::_shared_ptr < HeapWrapperPage > > _array_page;
    D3D12_DESCRIPTOR_HEAP_DESC _desc;
};
