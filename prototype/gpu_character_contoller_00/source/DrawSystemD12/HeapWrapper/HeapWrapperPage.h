#pragma once

class HeapWrapperPage
{
public:
   static std::shared_ptr<HeapWrapperPage> Factory(
      const int frameCount,
      const D3D12_DESCRIPTOR_HEAP_DESC& desc,
      ID3D12Device2* const pDevice
      );

   HeapWrapperPage(
      const int frameCount,
      const int numDescriptors
      //const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pDescriptorHeap,
      //const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHeapStart,
      //const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHeapStart
      );

   void OnDeviceLost();
   void OnDeviceRestored(const D3D12_DESCRIPTOR_HEAP_DESC& desc, ID3D12Device2* const pDevice);

   const int GetFreeIndex(
      const int length = 1
      );
   void FreeIndex(const int index, const int length = 1);

   D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const int index, const int frameIndex);
   D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const int index, const int frameIndex);

   ID3D12DescriptorHeap* const GetHeap(const int frameIndex);

private:
   static const size_t MAX_BACK_BUFFER_COUNT = 3;
   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescriptorHeap[MAX_BACK_BUFFER_COUNT];
   int m_frameCount;
   UINT m_handleIncrementSize;
   D3D12_CPU_DESCRIPTOR_HANDLE m_CPUHeapStart[MAX_BACK_BUFFER_COUNT];
   D3D12_GPU_DESCRIPTOR_HANDLE m_GPUHeapStart[MAX_BACK_BUFFER_COUNT];
   int m_indexHighWater;
   std::vector< int > m_freeIndex;
   int m_numDescriptors;

};
