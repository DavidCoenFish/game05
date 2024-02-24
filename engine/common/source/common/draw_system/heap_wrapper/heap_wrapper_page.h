#pragma once
class HeapWrapperPage
{
public:
    static std::shared_ptr < HeapWrapperPage > Factory(
        const D3D12_DESCRIPTOR_HEAP_DESC& in_desc,
        ID3D12Device2* const in_device
        );
    HeapWrapperPage(
        const int in_num_descriptors // Const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pDescriptorHeap,
        // Const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHeapStart,
        // Const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHeapStart

        );
    void OnDeviceLost();
    void OnDeviceRestored(
        const D3D12_DESCRIPTOR_HEAP_DESC&in_desc,
        ID3D12Device2* const in_device
        );
    const int GetFreeIndex(const int in_length = 1);
    void FreeIndex(
        const int in_index,
        const int in_length = 1
        );
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(
        const int in_index
        );
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(
        const int in_index
        );
    ID3D12DescriptorHeap* const GetHeap();

private:
    Microsoft::WRL::ComPtr < ID3D12DescriptorHeap > _descriptor_heap;
    UINT _handle_increment_size;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_heap_start;
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_heap_start;
    int _index_high_water;
    std::vector < int > _free_index;
    int _num_descriptors;
};
