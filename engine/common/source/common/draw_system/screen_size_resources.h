#pragma once
struct CD3DX12_CPU_DESCRIPTOR_HANDLE;
class RenderTargetBackBuffer;
class DrawSystem;
class IRenderTarget;
struct RenderTargetFormatData;
struct RenderTargetDepthData;

class ScreenSizeResources
{
public:
    ScreenSizeResources(
        DrawSystem* const in_draw_system,
        const Microsoft::WRL::ComPtr < ID3D12Device >&in_device,
        const Microsoft::WRL::ComPtr < IDXGIFactory6 >&in_dxgi_factory,
        const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue,
        const HWND in_hwnd,
        const UINT64 in_fence_value,
        const unsigned int in_back_buffer_count,
        const int in_width,
        const int in_height,
        const bool in_allow_tearing,
        // Const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
        // Const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT
        const RenderTargetFormatData&in_target_format_data,
        const RenderTargetDepthData&in_target_depth_data
        );
    ~ScreenSizeResources();
    const UINT64 GetFenceValue();
    void SetFenceValue(const UINT64 in_value);
    void Prepare(ID3D12GraphicsCommandList*&in_command_list);
    // Void Clear();
    const bool Present(
        HRESULT&in_hr,
        const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue
        );
    IRenderTarget* GetRenderTargetBackBuffer();
    void UpdateBackBufferIndex();
    const int GetBackBufferIndex() const
    {
        return back_buffer_index;
    }

    const int GetWidth() const
    {
        return width;
    }

    const int GetHeight() const
    {
        return height;
    }

    void WaitForGpu(const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue) noexcept;
    void MoveToNextFrame(const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue);

private:
    // CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
    // CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
    static const size_t MAX_BACK_BUFFER_COUNT = 3;
    int _width;
    int _height;
    bool _allow_tearing;
    int _back_buffer_count;
    int _back_buffer_index;
    Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList > _command_list;
    Microsoft::WRL::ComPtr < ID3D12CommandAllocator > _command_allocators[MAX_BACK_BUFFER_COUNT];
    UINT64 _fence_values[MAX_BACK_BUFFER_COUNT];
    Microsoft::WRL::ComPtr < ID3D12Fence > _fence;
    Microsoft::WRL::Wrappers::Event _fence_event;
    Microsoft::WRL::ComPtr < IDXGISwapChain3 > _swap_chain;
    std::_unique_ptr < RenderTargetBackBuffer > _render_target_back_buffer[MAX_BACK_BUFFER_COUNT];
};
