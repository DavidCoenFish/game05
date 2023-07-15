#include "common/common_pch.h"

#include "common/draw_system/d3dx12.h"
#include "common/draw_system/render_target/render_target_backbuffer.h"
#include "common/draw_system/screen_size_resources.h"

static const DXGI_FORMAT NoSRGB(DXGI_FORMAT in_fmt) noexcept
{
    switch (in_fmt)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8X8_UNORM;
    default:
        return in_fmt;
    }

}

ScreenSizeResources::ScreenSizeResources(
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
    const RenderTargetFormatData&in_target_format_data,
    const RenderTargetDepthData&in_target_depth_data
    ) 
    : back_buffer_count(in_back_buffer_count)
    , back_buffer_index(0)
    , allow_tearing(in_allow_tearing)
    , width(std::max(
        in_width,
        1
        ))
    , height(std::max(
        in_height,
        1
        ))
{
    for (unsigned int index = 0; index < MAX_BACK_BUFFER_COUNT;++ index)
    {
        fence_values[index] = in_fence_value;
    }
    // Create a command allocator for each back buffer that will be rendered to.
    for (int n = 0; n < back_buffer_count; n++)
    {
        DX::ThrowIfFailed(in_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(command_allocators[n].ReleaseAndGetAddressOf())
            ));
        wchar_t name[25] = {};
        swprintf_s(
            name,
            L"Command Allocators %u",
            n
            );
        command_allocators[n]->SetName(name);
    }
    // Create a command list for recording graphics commands.
    DX::ThrowIfFailed(in_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        command_allocators[0].Get(),
        nullptr,
        IID_PPV_ARGS(command_list.ReleaseAndGetAddressOf())
        ));
    DX::ThrowIfFailed(command_list->Close());

    {
        static int count = - 1;
        count += 1;
        wchar_t name[25] = {};
        swprintf_s(
            name,
            L"CommandList:%d",
            count
            );
        command_list->SetName(name);
    }
    const DXGI_FORMAT back_buffer_format = NoSRGB(in_target_format_data._format);

    {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
        swap_chain_desc.Width = width;
        swap_chain_desc.Height = height;
        swap_chain_desc.Format = back_buffer_format;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = back_buffer_count;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swap_chain_desc.Flags = allow_tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fs_swap_chain_desc = {};
        fs_swap_chain_desc.Windowed = TRUE;
        // Create a swap chain for the window.
        Microsoft::WRL::ComPtr < IDXGISwapChain1 > swap_chain;
        DX::ThrowIfFailed(in_dxgi_factory->CreateSwapChainForHwnd(
            in_command_queue.Get(),
            in_hwnd,
            &swap_chain_desc,
            &fs_swap_chain_desc,
            nullptr,
            swap_chain.GetAddressOf()
            ));
        DX::ThrowIfFailed(swap_chain.As(&swap_chain));

            // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
        DX::ThrowIfFailed(in_dxgi_factory->MakeWindowAssociation(
            in_hwnd,
            DXGI_MWA_NO_ALT_ENTER
            ));
    }
    // Handle color space settings for HDR
    // UpdateColorSpace();
    // Obtain the back buffers for this window which will be the final render targets
    // And create render target views for each of them.
    // RenderTargetFormatData targetFormatData(_backBufferFormat, true, VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f));
    // RenderTargetDepthData targetDepthData(depthBufferFormat, true, 1.0f);
    for (int n = 0; n < back_buffer_count; n++)
    {
        render_target_back_buffer[n] = std::make_unique < RenderTargetBackBuffer > (
            in_draw_system,
            in_device.Get(),
            n,
            in_target_format_data,
            in_target_depth_data,
            swap_chain.Get(),
            width,
            height
            );
    }
    // Reset the index to the current back buffer.
    back_buffer_index = swap_chain->GetCurrentBackBufferIndex();

    {
        // Create a fence for tracking GPU execution progress.
        DX::ThrowIfFailed(in_device->CreateFence(
            in_fence_value,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(fence.ReleaseAndGetAddressOf())
            ));
        SetFenceValue(in_fence_value + 1);
        fence->SetName(L"Fence");
    }
    fence_event.Attach(CreateEventEx(
        nullptr,
        nullptr,
        0,
        EVENT_MODIFY_STATE | SYNCHRONIZE
        ));
    if (!fence_event.IsValid())
    {
        throw std::exception("CreateEvent");
    }
}

ScreenSizeResources::~ScreenSizeResources() {} const UINT64 ScreenSizeResources::GetFenceValue()
{
    return fence_values[back_buffer_index];
}

void ScreenSizeResources::SetFenceValue(const UINT64 in_value)
{
    fence_values[back_buffer_index] = in_value;
    return;
}

void ScreenSizeResources::Prepare(ID3D12GraphicsCommandList*&in_command_list)
{
    // Reset command list and allocator.
    DX::ThrowIfFailed(in_command_allocators[back_buffer_index]->Reset());
    DX::ThrowIfFailed(in_command_list->Reset(
        in_command_allocators[back_buffer_index].Get(),
        nullptr
        ));
    in_command_list = in_command_list.Get();
    return;
}

void ScreenSizeResources::WaitForGpu(const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue) noexcept
{
    if (in_command_queue && fence && fence_event.IsValid())
    {
        // Schedule a Signal command in the GPU queue.
        UINT64 fence_value = GetFenceValue();
        if (SUCCEEDED(in_command_queue->Signal(
            fence.Get(),
            fence_value
            )))
        {
            // Wait until the Signal has been processed.
            if (SUCCEEDED(fence->SetEventOnCompletion(
                fence_value,
                fence_event.Get()
                )))
            {
                WaitForSingleObjectEx(
                    fence_event.Get(),
                    INFINITE,
                    FALSE
                    );
                // Increment the fence value for the current frame.
                SetFenceValue(fence_value + 1);
            }
        }
    }
    return;
}

const bool ScreenSizeResources::Present(
    HRESULT&in_hr,
    const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue
    )
{
    ((IRenderTarget*) render_target_back_buffer[back_buffer_index].get()) ->EndRender(in_command_list.Get());
    // Send the command list off to the GPU for processing.
    DX::ThrowIfFailed(in_command_list->Close());
    in_command_queue->ExecuteCommandLists(
        1,
        CommandListCast(in_command_list.GetAddressOf())
        );
    if (allow_tearing)
    {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        // Note this will fail if in true 'fullscreen' mode.
        in_hr = swap_chain->Present(
            0,
            DXGI_PRESENT_ALLOW_TEARING
            );
    }
    else
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // To sleep until the next VSync. This ensures we don't waste any cycles rendering
        // Frames that will never be displayed to the screen.
        in_hr = swap_chain->Present(
            1,
            0
            );
    }
    // If the device was reset we must completely reinitialize the renderer.
    if (in_hr == DXGI_ERROR_DEVICE_REMOVED || in_hr == DXGI_ERROR_DEVICE_RESET)
    {
        return false;
    }
    else
    {
        DX::ThrowIfFailed(in_hr);
    }
    return true;
}

IRenderTarget* ScreenSizeResources::GetRenderTargetBackBuffer()
{
    return render_target_back_buffer[back_buffer_index].get();
}

void ScreenSizeResources::UpdateBackBufferIndex()
{
    // Update the back buffer index.
    back_buffer_index = swap_chain->GetCurrentBackBufferIndex();
}

void ScreenSizeResources::MoveToNextFrame(const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue)
{
    // Schedule a Signal command in the queue.
    const UINT64 current_fence_value = GetFenceValue();
    DX::ThrowIfFailed(in_command_queue->Signal(
        fence.Get(),
        current_fence_value
        ));
    UpdateBackBufferIndex();
    const UINT64 next_back_buffer_fence_value = GetFenceValue();
    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (fence->GetCompletedValue() < next_back_buffer_fence_value)
    {
        DX::ThrowIfFailed(fence->SetEventOnCompletion(
            next_back_buffer_fence_value,
            fence_event.Get()
            ));
        WaitForSingleObjectEx(
            fence_event.Get(),
            INFINITE,
            FALSE
            );
    }
    // Set the fence value for the next frame.
    SetFenceValue(current_fence_value + 1);
}

