#pragma once

#include "Common/DrawSystem/Geometry/IGeometry.h"
#include "Common/DrawSystem/IResource.h"

template <typename TypeVertex >
class Geometry : public IResource
{
public:
   Geometry(
      DrawSystem* const pDrawSystem,
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
      const std::vector< TypeVertex >& vertexData
      )
      : IResource(pDrawSystem)
      , m_primitiveTopology(primitiveTopology)
      , m_inputElementDescArray(inputElementDescArray)
      , m_vertexData(vertexData)
      , m_pVertexBuffer()
      , m_vertexBufferView()
   {
      //nop
   }

   void Draw(ID3D12GraphicsCommandList* const pCommandList)
   {
      IGeometry::DrawImplementation(
         pCommandList,
         (int)m_vertexData.size(),
         m_primitiveTopology,
         m_vertexBufferView
         );
   }

private:
   virtual void OnDeviceLost() override
   {
      IGeometry::DeviceLostImplementation(m_pVertexBuffer);
   }

   virtual void OnDeviceRestored(
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device2* const pDevice
      ) override
   {
      IGeometry::DeviceRestoredImplementation(
         m_pDrawSystem,
         pCommandList,
         pDevice,
         (int)m_vertexData.size(),
         sizeof(TypeVertex),
         m_pVertexBuffer,
         m_vertexBufferView,
         m_vertexData.data()
         );
   }

private:
   const D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology;
   const std::vector< D3D12_INPUT_ELEMENT_DESC > m_inputElementDescArray;
   std::vector< TypeVertex > m_vertexData;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexBuffer;
   D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

};
