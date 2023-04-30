#include "pch.h"

#include "DrawSystemD12/Shader/UnorderedAccessInfo.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapperItem.h"

UnorderedAccessInfo::UnorderedAccessInfo(
   const std::shared_ptr< HeapWrapperItem >& pUnorderedAccessViewHandle,
   const D3D12_SHADER_VISIBILITY visiblity
   )
   : m_pUnorderedAccessViewHandle(pUnorderedAccessViewHandle)
   , m_visiblity(visiblity)
{
   return;
}

void UnorderedAccessInfo::SetUnorderedAccessViewHandle(const std::shared_ptr< HeapWrapperItem >& pUnorderedAccessViewHandle)
{
   m_pUnorderedAccessViewHandle = pUnorderedAccessViewHandle;
   return;
}

void UnorderedAccessInfo::Activate( 
   ID3D12GraphicsCommandList* const pCommandList,
   const int rootParamIndex
   )
{
   if (m_pUnorderedAccessViewHandle)
   {
      auto heap = m_pUnorderedAccessViewHandle->GetHeap();
      pCommandList->SetDescriptorHeaps(1, &heap);
      pCommandList->SetComputeRootDescriptorTable(
         rootParamIndex, 
         m_pUnorderedAccessViewHandle->GetGPUHandle()
         );
   }

   return;
}


