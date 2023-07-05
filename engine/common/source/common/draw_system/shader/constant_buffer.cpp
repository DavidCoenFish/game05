#include "CommonPCH.h"

#include "Common/DrawSystem/Shader/ConstantBuffer.h"
#include "Common/DrawSystem/HeapWrapper/HeapWrapperItem.h"
#include "Common/DrawSystem/d3dx12.h"

ConstantBuffer::ConstantBuffer(
   const int frameCount, 
   const size_t constantBufferSize,
   const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
   const void* const pData,
   const D3D12_SHADER_VISIBILITY visiblity
   )
   : m_frameCount(frameCount)
   , m_constantBufferSize(constantBufferSize)
   , m_pHeapWrapperItem(pHeapWrapperItem)
   , m_pData(pData)
   , m_visiblity(visiblity)
{
   for (int i = 0; i < MAX_BACK_BUFFER_COUNT; ++i)
   {
      m_pGPUAddress[i] = 0;
   }
}

void ConstantBuffer::DeviceLost()
{
   for (int i = 0; i < m_frameCount; ++i)
   {
      m_constantBufferUploadHeap[i].Reset();
      m_pGPUAddress[i] = 0;
   }
   return;
}

void ConstantBuffer::DeviceRestored(ID3D12Device* const pDevice)
{
   for (int i = 0; i < m_frameCount; ++i)
   {
      const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
      const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(1024 * 64);
      DX::ThrowIfFailed(pDevice->CreateCommittedResource(
         &heapProperties, // this heap will be used to upload the constant buffer data
         D3D12_HEAP_FLAG_NONE, // no flags
         &resourceDesc, // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
         D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
         nullptr, // we do not have use an optimized clear value for constant buffers
         IID_PPV_ARGS(&m_constantBufferUploadHeap[i])
         ));
      m_constantBufferUploadHeap[i]->SetName(L"Constant Buffer Upload Resource Heap");

      D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
      cbvDesc.BufferLocation = m_constantBufferUploadHeap[i]->GetGPUVirtualAddress();
      cbvDesc.SizeInBytes = (m_constantBufferSize + 255) & ~255;    // CB size is required to be 256-byte aligned.
      pDevice->CreateConstantBufferView(&cbvDesc, m_pHeapWrapperItem->GetCPUHandleFrame(i) );
         //mainDescriptorHeap[i]->GetCPUDescriptorHandleForHeapStart());

        //ZeroMemory(&cbColorMultiplierData, sizeof(cbColorMultiplierData));

      CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
      DX::ThrowIfFailed(m_constantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_pGPUAddress[i])));
        //memcpy(m_pGPUAddress[i], &cbColorMultiplierData, sizeof(cbColorMultiplierData));
    }
}

void ConstantBuffer::Activate(
   ID3D12GraphicsCommandList* const pCommandList,
   //void* const pData, 
   const int rootParamIndex,
   const int frameIndex
   )
{
   ID3D12DescriptorHeap* descriptorHeaps[] = { m_pHeapWrapperItem->GetHeap() };
   pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
   pCommandList->SetGraphicsRootDescriptorTable(rootParamIndex, m_pHeapWrapperItem->GetGPUHandle() );

   if (0 == m_pGPUAddress[frameIndex])
   {
      return;
   }
   memcpy(m_pGPUAddress[frameIndex], m_pData, m_constantBufferSize);
   return;
}

const int ConstantBuffer::GetNum32BitValues() const
{
   return (int)(m_constantBufferSize / 4);
}
