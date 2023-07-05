#include "CommonPCH.h"

#include "Common/DrawSystem/HeapWrapper/HeapWrapperPage.h"
#include "Common/DrawSystem/d3dx12.h"

std::shared_ptr<HeapWrapperPage> HeapWrapperPage::Factory(
   const int frameCount,
   const D3D12_DESCRIPTOR_HEAP_DESC& desc,
   ID3D12Device2* const pDevice
   )
{
   auto pResult = std::make_shared< HeapWrapperPage >(frameCount, desc.NumDescriptors);
   pResult->OnDeviceRestored( desc, pDevice );
   return pResult;
}

HeapWrapperPage::HeapWrapperPage(
   const int frameCount,
   const int numDescriptors
   //const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pDescriptorHeap,
   //const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHeapStart,
   //const D3D12_GPU_DESCRIPTOR_HANDLE& GPUHeapStart
   )
   : m_frameCount(frameCount)
   , m_pDescriptorHeap() //pDescriptorHeap)
   , m_handleIncrementSize(0)
   , m_CPUHeapStart() //CPUHeapStart)
   , m_GPUHeapStart() //GPUHeapStart)
   , m_indexHighWater(0)
   , m_freeIndex()
   , m_numDescriptors(numDescriptors)
{
   return;
}

void HeapWrapperPage::OnDeviceLost()
{
   for (int index = 0; index < m_frameCount; ++index)
   {
      m_pDescriptorHeap[index].Reset();
   }
}

void HeapWrapperPage::OnDeviceRestored(const D3D12_DESCRIPTOR_HEAP_DESC& desc, ID3D12Device2* const pDevice)
{
   if (nullptr == pDevice)
   {
      return;
   }
   for (int index = 0; index < m_frameCount; ++index)
   {
      if (nullptr != m_pDescriptorHeap[index])
      {
         return;
      }
      DX::ThrowIfFailed(
         pDevice->CreateDescriptorHeap(
            &desc, 
            IID_PPV_ARGS(m_pDescriptorHeap[index].ReleaseAndGetAddressOf())
            )
         );
      m_CPUHeapStart[index] = m_pDescriptorHeap[index]->GetCPUDescriptorHandleForHeapStart();
      m_GPUHeapStart[index] = m_pDescriptorHeap[index]->GetGPUDescriptorHandleForHeapStart();
   }

   m_handleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(desc.Type);
}

const int HeapWrapperPage::GetFreeIndex(
   const int length
   )
{
   if ((1 == length) && (0 < m_freeIndex.size()))
   {
      int index = m_freeIndex.back();
      m_freeIndex.pop_back();
      return index;
   }
   if (m_indexHighWater + length <= m_numDescriptors)
   {
      int result = m_indexHighWater;
      m_indexHighWater += length;
      return result;
   }
   return -1;
}

void HeapWrapperPage::FreeIndex(const int trace, const int length)
{
   for (int index = 0; index < length; ++index)
   {
      m_freeIndex.push_back(trace + index);
   }
   if (m_numDescriptors == m_freeIndex.size())
   {
      m_indexHighWater = 0;
      m_freeIndex.clear();
   }
   return;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapWrapperPage::GetCPUHandle(const int index, const int frameIndex)
{
   const int localFrameIndex = frameIndex % m_frameCount;
   return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CPUHeapStart[localFrameIndex], index, m_handleIncrementSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapWrapperPage::GetGPUHandle(const int index, const int frameIndex)
{
   const int localFrameIndex = frameIndex % m_frameCount;
   return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_GPUHeapStart[localFrameIndex], index, m_handleIncrementSize);
}

ID3D12DescriptorHeap* const HeapWrapperPage::GetHeap(const int frameIndex)
{
   const int localFrameIndex = frameIndex % m_frameCount;
   return m_pDescriptorHeap[localFrameIndex].Get();
}
