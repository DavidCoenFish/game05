#include "CommonPCH.h"

#include "Common/DrawSystem/Geometry/IGeometry.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/d3dx12.h"
#include "Common/DirectXTK12/GraphicsMemory.h"


void IGeometry::DrawImplementation(
   ID3D12GraphicsCommandList* const pCommandList, 
   const UINT vertexCount, 
   const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
   D3D12_VERTEX_BUFFER_VIEW& vertexBufferView
   )
{
   PIXBeginEvent(pCommandList, PIX_COLOR_DEFAULT, L"Draw");

   pCommandList->IASetPrimitiveTopology(primitiveTopology);
   pCommandList->IASetVertexBuffers(
      0, 
      1, 
      &vertexBufferView
      );

   pCommandList->DrawInstanced(
      vertexCount,
      1, 
      0, 
      0
      );

   PIXEndEvent(pCommandList);
}

void IGeometry::DeviceLostImplementation(
   Microsoft::WRL::ComPtr<ID3D12Resource>& pVertexBuffer
   )
{
   pVertexBuffer.Reset();
}

void IGeometry::DeviceRestoredImplementation(
   DrawSystem* const pDrawSystem,
   ID3D12GraphicsCommandList* const pCommandList,
   ID3D12Device* const pDevice,
   const int vertexCount,
   const int byteVertexSize,
   Microsoft::WRL::ComPtr<ID3D12Resource>& pVertexBuffer,
   D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
   void* pRawData
   )
{
   const int byteTotalSize = byteVertexSize * vertexCount;
   auto bufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteTotalSize);

   auto pUploadMemory = pDrawSystem->AllocateUpload(byteTotalSize);

   //create memory on gpu for vertex buffer
   {
      auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
      pDevice->CreateCommittedResource(
         &heapProperties, // a default heap
         D3D12_HEAP_FLAG_NONE, // no flags
         &bufferResourceDesc, // resource description for a buffer
         D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
                                          // from the upload heap to this heap
         nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
         IID_PPV_ARGS(pVertexBuffer.ReleaseAndGetAddressOf())
         );
      pVertexBuffer->SetName(L"GeometryVertexBuffer");
   }

   {
      if (pCommandList)
      {
         D3D12_SUBRESOURCE_DATA vertexData = {};
         vertexData.pData = pRawData;
         vertexData.RowPitch = byteTotalSize;
         vertexData.SlicePitch = byteTotalSize;

         UpdateSubresources(
            pCommandList,
            pVertexBuffer.Get(), 
            pUploadMemory.Resource(), 
            pUploadMemory.ResourceOffset(), 
            0,
            1,
            &vertexData
            );
      }

      auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
      pCommandList->ResourceBarrier(1, &barrier);
   }

   vertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
   vertexBufferView.StrideInBytes = byteVertexSize;
   vertexBufferView.SizeInBytes = byteTotalSize;

   return;
}
