#pragma once

#include "DrawSystemD12/IResource.h"

class GeometryGeneric : public IResource
{
public:
   GeometryGeneric(
      DrawSystem* const pDrawSystem,
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
      const std::vector< float >& vertexDataRaw,
      const int floatPerVertex
      );

   void Draw(ID3D12GraphicsCommandList* const pCommandList);

private:
   virtual void OnDeviceLost() override;
   virtual void OnDeviceRestored(
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device2* const pDevice
      ) override;

private:
   const D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology;
   const std::vector< D3D12_INPUT_ELEMENT_DESC > m_inputElementDescArray;
   std::vector< float > m_vertexRawData;
   int m_floatPerVertex;

   Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexBuffer;
   D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

};
