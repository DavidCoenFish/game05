#include "pch.h"

#include "DrawSystemD12/Shader/UnorderedAccess.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapperItem.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/d3dx12.h"
//#include "DirectXTK12/DirectXHelpers.h"
//#include "DirectXTK12/GraphicsMemory.h"

UnorderedAccess::UnorderedAccess(
   DrawSystem* const pDrawSystem,
   const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
   const D3D12_RESOURCE_DESC& desc, 
   const D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessViewDesc
   )
   : IResource( pDrawSystem )
   , m_pHeapWrapperItem(pHeapWrapperItem)
   , m_desc(desc)
   , m_unorderedAccessViewDesc(unorderedAccessViewDesc)
{
   return;
}

void UnorderedAccess::OnDeviceLost()
{
   m_pResource.Reset();
}

std::shared_ptr< HeapWrapperItem > UnorderedAccess::GetHeapWrapperItem() const
{
   return m_pHeapWrapperItem;
}

void UnorderedAccess::OnDeviceRestored(
   ID3D12GraphicsCommandList* const,
   ID3D12Device2* const pDevice
   )
{
   CD3DX12_HEAP_PROPERTIES heapDefault(D3D12_HEAP_TYPE_DEFAULT);
   pDevice->CreateCommittedResource(
      &heapDefault,
      D3D12_HEAP_FLAG_NONE,
      &m_desc,
      D3D12_RESOURCE_STATE_COMMON,
      nullptr,
      IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
      );

   pDevice->CreateUnorderedAccessView( 
      m_pResource.Get(), 
      nullptr, 
      &m_unorderedAccessViewDesc, 
      m_pHeapWrapperItem->GetCPUHandleFrame(0)
      );

   return;
}

