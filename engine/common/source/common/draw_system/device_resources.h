#pragma once
#include "common/draw_system/render_target/render_target_depth_data.h"
#include "common/draw_system/render_target/render_target_format_data.h"

// Enum class DeviceResourcesOptions
// {
// None = 0,
// AllowTearing = 0x1,
// EnableHDR = 0x2
// };
class DrawSystem;
class IResource;
class ScreenSizeResources;
class IRenderTarget;
struct RenderTargetFormatData;
struct RenderTargetDepthData;

namespace DirectX
{
    class GraphicsMemory;
    class GraphicsResource;

};
class DeviceResources
{
public:
    DeviceResources(
        const unsigned int in_back_buffer_count,
        const D3D_FEATURE_LEVEL in_d3d_feature_level,
        const unsigned int in_options,
        const RenderTargetFormatData&in_target_format_data,
        const RenderTargetDepthData&in_target_depth_data
        );
    ~DeviceResources();
    void WaitForGpu() noexcept;
    const bool OnResize(
        DrawSystem* const in_draw_system,
        const HWND in_hwnd,
        int&out_width,
        int&out_height
        );
    const int GetBackBufferIndex() const;
    DirectX::GraphicsResource AllocateUpload(
        const std::size_t in_size,
        void* const in_data_or_nullptr,
        size_t in_alignment = 16
        );
    void Prepare(ID3D12GraphicsCommandList*&in_command_list);
    // Void Clear();
    const bool Present();
    IRenderTarget* GetRenderTargetBackBuffer();
    ID3D12Device2* const GetD3dDevice();
    // ID3D12CommandQueue* const GetCommandQueue();
    ID3D12GraphicsCommandList* GetCustomCommandList();
    void CustomCommandListFinish(ID3D12GraphicsCommandList* in_command_list);
    // Return true if size changed
    const bool CreateWindowSizeDependentResources(
        DrawSystem* const in_draw_system,
        const HWND in_hwnd,
        int* out_width = nullptr,
        int* out_height = nullptr
        );

private:
    void GetAdapter(
        IDXGIAdapter1** in_pp_adapter,
        const D3D_FEATURE_LEVEL in_d3d_feature_level
        );
    void MoveToNextFrame();
    // Void UpdateColorSpace();
    void WaitForCustomCommand();

public:
    static const unsigned int c_allow_tearing = 0x1;
    static const unsigned int c_enable_hdr = 0x2;

private:
    unsigned int _back_buffer_count;
    unsigned int _options;
    RenderTargetFormatData _target_format_data;
    RenderTargetDepthData _target_depth_data;
    Microsoft::WRL::ComPtr < IDXGIFactory6 > _dxgi_factory;
    DWORD _dxgi_factory_flags;
    Microsoft::WRL::ComPtr < ID3D12Device2 > _device;
    Microsoft::WRL::ComPtr < ID3D12CommandQueue > _command_queue;
    UINT64 _custom_command_list_fence_value;
    Microsoft::WRL::ComPtr < ID3D12Fence > _custom_command_fence;
    Microsoft::WRL::Wrappers::Event _custom_command_fence_event;
    Microsoft::WRL::ComPtr < ID3D12CommandAllocator > _custom_command_allocator;
    Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList > _custom_command_list;
    std::_unique_ptr < ScreenSizeResources > _screen_size_resources;
    std::_unique_ptr < DirectX::GraphicsMemory > _graphics_memory;
};
