#include "pch.h"

#include "DrawSystemD12/DrawSystemFrame.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/d3dx12.h"
#include "DrawSystemD12/RenderTarget/IRenderTarget.h"
#include "DrawSystemD12/Shader/Shader.h"
#include "DrawSystemD12/Geometry/GeometryGeneric.h"

DrawSystemFrame::DrawSystemFrame(DrawSystem& drawSystem)
   : m_drawSystem(drawSystem)
   , m_pCommandList(nullptr)
   , m_pRenderTarget(nullptr)
{
   m_drawSystem.Prepare(
      m_pCommandList
      );
   //m_drawSystem.Clear();
}

DrawSystemFrame::~DrawSystemFrame()
{
   //if (m_pCommandList)
   //{
   //   DX::ThrowIfFailed(m_pCommandList->Close());
   //}
   //if (m_pCommandList && m_pCommandQueue)
   //{
   //   m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(m_pCommandList.GetAddressOf()));
   //}
   SetRenderTarget(nullptr);

   m_drawSystem.Present();
}

ID3D12GraphicsCommandList* DrawSystemFrame::GetCommandList()
{
   return m_pCommandList;
}

const int DrawSystemFrame::GetBackBufferIndex()
{
   return m_drawSystem.GetBackBufferIndex();
}

void DrawSystemFrame::SetRenderTarget(IRenderTarget* const pRenderTarget)
{
   if (m_pRenderTarget)
   {
      m_pRenderTarget->EndRender(m_pCommandList);
   }
   m_pRenderTarget = pRenderTarget;
   if (m_pRenderTarget)
   {
      m_pRenderTarget->StartRender(m_pCommandList);
   }
}

void DrawSystemFrame::SetShader(Shader* const pShader)
{
   pShader->SetActivate(m_pCommandList, m_drawSystem.GetBackBufferIndex());
}

void DrawSystemFrame::Draw(GeometryGeneric* const pGeometry)
{
   pGeometry->Draw(m_pCommandList);
}

void DrawSystemFrame::Dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ)
{
   m_pCommandList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
}

void DrawSystemFrame::ResourceBarrier(IResource* const pResource, D3D12_RESOURCE_STATES afterState)
{
	pResource->OnResourceBarrier(m_pCommandList, afterState);
}
