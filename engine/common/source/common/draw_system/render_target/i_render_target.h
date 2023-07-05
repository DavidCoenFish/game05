#pragma once
//#include "Common/DrawSystem/IResource.h"

class IRenderTarget// : public IResource
{
public:
   IRenderTarget(); //DrawSystem* const pDrawSystem);
   virtual ~IRenderTarget();

   virtual void StartRender(ID3D12GraphicsCommandList* const pCommandList) = 0;
   virtual void EndRender(ID3D12GraphicsCommandList* const pCommandList) = 0;

   //virtual void GetPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) const = 0;
   //virtual DXGI_FORMAT GetDepthFormat() const;
   virtual void GetFormatData(
      DXGI_FORMAT& depthFormat,
      int& renderTargetViewFormatCount,
      const DXGI_FORMAT*& pRenderTargetViewFormat
      ) const = 0;

   virtual const int GetWidth() const = 0;
   virtual const int GetHeight() const = 0;

};
