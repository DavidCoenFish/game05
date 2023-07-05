#pragma once

#include "Common/DrawSystem/Geometry/Geometry.h"

class DrawSystem;
class IRenderTarget;
class Shader;
class GeometryGeneric;

class DrawSystemFrame
{
public:
   DrawSystemFrame(DrawSystem& drawSystem);
   ~DrawSystemFrame();

   ID3D12GraphicsCommandList* GetCommandList();
   const int GetBackBufferIndex();

   void SetRenderTarget(IRenderTarget* const pRenderTarget);
   void SetShader(Shader* const pShader);
   void Draw(GeometryGeneric* const pGeometry);
   void Dispatch(uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1);

   template <typename TypeVertex >
   void Draw(Geometry<TypeVertex>* const pGeometry)
   {
      pGeometry->Draw(m_pCommandList);
   }

private:
   DrawSystem& m_drawSystem;
   ID3D12GraphicsCommandList* m_pCommandList;
   ID3D12CommandQueue* m_pCommandQueue;
   IRenderTarget* m_pRenderTarget;

};
