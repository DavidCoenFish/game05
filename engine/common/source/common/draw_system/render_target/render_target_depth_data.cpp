#include "CommonPCH.h"

#include "Common/DrawSystem/RenderTarget/RenderTargetDepthData.h"

RenderTargetDepthData::RenderTargetDepthData(
   const DXGI_FORMAT _format,
   const bool _clearDepthOnSet,
   const float _clearDepth,
   const bool _clearStencilOnSet,
   const UINT8 _clearStencil,
   const bool _shaderResource
   )
   : format(_format)
   , clearDepthOnSet(_clearDepthOnSet)
   , clearDepth(_clearDepth)
   , clearStencilOnSet(_clearStencilOnSet)
   , clearStencil(_clearStencil)
   , shaderResource(_shaderResource)
{
   return;
}

D3D12_CLEAR_VALUE RenderTargetDepthData::MakeClearValue() const
{
   D3D12_CLEAR_VALUE result;
   result.Format = format;
   result.DepthStencil.Depth = clearDepth;
   result.DepthStencil.Stencil = clearStencil;
   return result;
}

