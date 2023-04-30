#pragma once

struct RenderTargetDepthData
{
   explicit RenderTargetDepthData(
      const DXGI_FORMAT _format = DXGI_FORMAT_UNKNOWN,
      const bool _clearDepthOnSet = false,
      const float _clearDepth = 1.0f,
      const bool _clearStencilOnSet = false,
      const UINT8 _clearStencil = 0,
      const bool _shaderResource = false
      );
   D3D12_CLEAR_VALUE MakeClearValue() const;

   DXGI_FORMAT format;
   bool clearDepthOnSet;
   float clearDepth;
   bool clearStencilOnSet;
   UINT8 clearStencil;
   bool shaderResource;

};
