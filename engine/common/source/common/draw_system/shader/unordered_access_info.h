#pragma once

#include "Common/DrawSystem/d3dx12.h"

class HeapWrapperItem;

struct UnorderedAccessInfo
{
public:
   explicit UnorderedAccessInfo(
      const std::shared_ptr< HeapWrapperItem >& pUnorderedAccessViewHandle = nullptr,
      const D3D12_SHADER_VISIBILITY visiblity = D3D12_SHADER_VISIBILITY_ALL
      );

   void SetUnorderedAccessViewHandle(const std::shared_ptr< HeapWrapperItem >& pUnorderedAccessViewHandle);
   const D3D12_SHADER_VISIBILITY GetVisiblity() const { return m_visiblity; }

   void Activate( 
      ID3D12GraphicsCommandList* const pCommandList,
      const int rootParamIndex
      );

private:
   std::shared_ptr< HeapWrapperItem > m_pUnorderedAccessViewHandle;
   D3D12_SHADER_VISIBILITY m_visiblity;
};
