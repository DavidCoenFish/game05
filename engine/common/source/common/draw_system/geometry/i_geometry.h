#pragma once

//#include "Common/DrawSystem/IResource.h"
class DrawSystem;

class IGeometry// : public IResource
{
public:
   //IGeometry(DrawSystem* const pDrawSystem);
   //virtual ~IGeometry();
   //virtual void Draw(ID3D12GraphicsCommandList* const pCommandList) = 0; 

   //protected:
   static void DrawImplementation(
      ID3D12GraphicsCommandList* const pCommandList, 
      const UINT vertexCount, 
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      D3D12_VERTEX_BUFFER_VIEW& vertexBufferView
      );

   static void DeviceLostImplementation(
      Microsoft::WRL::ComPtr<ID3D12Resource>& pVertexBuffer
      );

   static void DeviceRestoredImplementation(
      DrawSystem* const pDrawSystem,
      ID3D12GraphicsCommandList* const pCommandList,
      ID3D12Device* const pDevice,
      const int vertexCount,
      const int byteVertexSize,
      Microsoft::WRL::ComPtr<ID3D12Resource>& pVertexBuffer,
      D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
      void* pRawData
      );
};
