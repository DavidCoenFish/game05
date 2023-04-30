#include "pch.h"

#include "DrawSystemD12/RenderTarget/RenderTargetFormatData.h"

RenderTargetFormatData::RenderTargetFormatData(
   const DXGI_FORMAT _format,
   const bool _clearOnSet,
   const VectorFloat4& _clearColor
   )
   : format(_format)
   , clearOnSet(_clearOnSet)
   , clearColor(_clearColor)
{
   return;
}

D3D12_CLEAR_VALUE RenderTargetFormatData::MakeClearValue() const
{
   D3D12_CLEAR_VALUE result({
      format,
      { 
         clearColor[0],
         clearColor[1],
         clearColor[2],
         clearColor[3]
      }});
   return result;
}

