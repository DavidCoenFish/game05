#include "CommonPCH.h"

#include "Common/DrawSystem/FrameCommandList.h"
#include "Common/DrawSystem/d3dx12.h"

CustomCommandList::CustomCommandList(
   ID3D12Device* const pDevice,
   ID3D12CommandQueue* const pCommandQueue,
   ID3D12GraphicsCommandList* const pCommandList
   )
   : m_pDevice(pDevice)
   , m_pCommandQueue(pCommandQueue)
   , m_pCommandList(pCommandList)
{
   return;
}

CustomCommandList::~CustomCommandList()
{
   // Send the command list off to the GPU for processing.
   if (m_pCommandList)
   {
      DX::ThrowIfFailed(m_pCommandList->Close());
   }
   if (m_pCommandQueue)
   {
      m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(&m_pCommandList));
   }

   return;
}

void CustomCommandList::UpdateSubresourcesMethod(
   ID3D12Resource* pDestinationResource,
   ID3D12Resource* pIntermediate,
   UINT64 IntermediateOffset,
   UINT FirstSubresource,
   UINT NumSubresources,
   D3D12_SUBRESOURCE_DATA* pSrcData,
   D3D12_BOX* pDestBoxOrNull
   )
{
   UpdateSubresources(
      m_pCommandList,
      pDestinationResource,
      pIntermediate,
      IntermediateOffset,
      FirstSubresource,
      NumSubresources,
      pSrcData);
   pDestBoxOrNull;
   //UpdateSubresources(
   //   m_pCommandList,
   //   pDestinationResource,
   //   pIntermediate,
   //   _In_range_(0,D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
   //   _In_range_(0,D3D12_REQ_SUBRESOURCES-FirstSubresource) UINT NumSubresources,
   //   UINT64 RequiredSize,
   //   _In_reads_(NumSubresources) const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
   //   _In_reads_(NumSubresources) const UINT* pNumRows,
   //   _In_reads_(NumSubresources) const UINT64* pRowSizesInBytes,
   //   _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA* pSrcData);

   //UpdateSubresources(
   //   m_pCommandList,
   //   pDestinationResource,
   //   pIntermediate,
   //   IntermediateOffset,
   //   FirstSubresource,
   //   NumSubresources,
   //   pSrcData,
   //   pDestBoxOrNull
   //   );
   return;
}

void CustomCommandList::ResourceBarrier(
   ID3D12Resource* const pResource,
   D3D12_RESOURCE_STATES stateBefore,
   D3D12_RESOURCE_STATES stateAfter
   )
{
   auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, stateBefore, stateAfter);
   m_pCommandList->ResourceBarrier(1, &barrier);
}

