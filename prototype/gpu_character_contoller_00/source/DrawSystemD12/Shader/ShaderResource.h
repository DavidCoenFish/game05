#pragma once

#include "DrawSystemD12/IResource.h"

class HeapWrapperItem;

/*
was ShaderTexture
*/
class ShaderResource : public IResource
{
public:
   ShaderResource(
      DrawSystem* const pDrawSystem,
      const std::shared_ptr< HeapWrapperItem >& shaderResource,
      const D3D12_RESOURCE_DESC& desc, 
      const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
      const std::vector<uint8_t>& data
      );

   std::shared_ptr< HeapWrapperItem > GetHeapWrapperItem() const;

   static void UploadCreateResource(
      DrawSystem* const pDrawSystem,
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device* const pDevice,
      Microsoft::WRL::ComPtr<ID3D12Resource>& pResource,
      std::shared_ptr< HeapWrapperItem >& shaderResource,
      const D3D12_RESOURCE_DESC& desc,
      const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
      const size_t dataSize,
      void* pData,
      LPCWSTR pName
      );

   //void WriteData(
   //   const int x,
   //   const int y,
   //   const size_t dataSizeBytes,
   //   void* pData
   //   );
   //std::vector<uint8_t>& GetData();
   //void UploadChangesIfNeeded(
   //   ID3D12GraphicsCommandList* const pCommandList
   //   );

private:
   virtual void OnDeviceLost() override;
   virtual void OnDeviceRestored(
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device2* const pDevice
      ) override;

private:
   Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
   std::shared_ptr< HeapWrapperItem > m_shaderResource;
   D3D12_RESOURCE_DESC m_desc;
   D3D12_SHADER_RESOURCE_VIEW_DESC m_shaderResourceViewDesc;
   std::vector<uint8_t> m_data;

  // bool m_bDataDirty;
  // //https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist-copytextureregion
  // D3D12_BOX m_dataDirtyBox;

};
