#pragma once

#include "Common/Math/VectorFloat4.h"

struct RenderTargetFormatData
{
   explicit RenderTargetFormatData(
      const DXGI_FORMAT _format = DXGI_FORMAT_UNKNOWN,
      const bool _clearOnSet = false,
      const VectorFloat4& _clearColor = VectorFloat4()
      );
   D3D12_CLEAR_VALUE MakeClearValue() const;

   DXGI_FORMAT format;
   bool clearOnSet;
   VectorFloat4 clearColor;
};
