#pragma once
// #include "Common/DrawSystem/IResource.h"
class IRenderTarget // : public IResource

{
public:
    IRenderTarget();
    // DrawSystem* const pDrawSystem);
    virtual ~IRenderTarget();
    virtual void StartRender(ID3D12GraphicsCommandList* const in_command_list) = 0;
    virtual void EndRender(ID3D12GraphicsCommandList* const in_command_list) = 0;
    // Virtual void GetPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) const = 0;
    // Virtual DXGI_FORMAT GetDepthFormat() const;
    virtual void GetFormatData(
        DXGI_FORMAT&in_depth_format,
        int&in_render_target_view_format_count,
        const DXGI_FORMAT*&in_render_target_view_format
        ) const = 0;
    virtual const int GetWidth() const = 0;
    virtual const int GetHeight() const = 0;
};
