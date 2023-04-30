#pragma once

#include "DrawSystemD12/IResource.h"

class HeapWrapperItem;

class UnorderedAccess : public IResource
{
public:
   UnorderedAccess(
      DrawSystem* const pDrawSystem,
      const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
      const D3D12_RESOURCE_DESC& desc, 
      const D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessViewDesc
      );

   std::shared_ptr< HeapWrapperItem > GetHeapWrapperItem() const;

private:
   virtual void OnDeviceLost() override;
   virtual void OnDeviceRestored(
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device2* const pDevice
      ) override;

private:
   Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
   std::shared_ptr< HeapWrapperItem > m_pHeapWrapperItem;
   D3D12_RESOURCE_DESC m_desc; 
   D3D12_UNORDERED_ACCESS_VIEW_DESC m_unorderedAccessViewDesc;
};
