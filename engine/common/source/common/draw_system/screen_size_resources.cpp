#include "CommonPCH.h"

#include "Common/DrawSystem/ScreenSizeResources.h"
#include "Common/DrawSystem/d3dx12.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetBackbuffer.h"

static const DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt) noexcept
{
   switch (fmt)
   {
   case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
   case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
   case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
   default:                                return fmt;
   }
}

ScreenSizeResources::ScreenSizeResources(
   DrawSystem* const pDrawSystem,
   const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice,
   const Microsoft::WRL::ComPtr<IDXGIFactory6>& pDXGIFactory,
   const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue,
   const HWND hWnd,
   const UINT64 fenceValue,
   const unsigned int backBufferCount,
   const int width,
   const int height,
   const bool bAllowTearing,
   const RenderTargetFormatData& targetFormatData,
   const RenderTargetDepthData& targetDepthData
   )
   : m_backBufferCount(backBufferCount)
   , m_backBufferIndex(0)
   , m_bAllowTearing(bAllowTearing)
   , m_width(std::max(width, 1))
   , m_height(std::max(height, 1))
{
   for (unsigned int index = 0; index < MAX_BACK_BUFFER_COUNT; ++index)
   {
      m_fenceValues[index] = fenceValue;
   }

   // Create a command allocator for each back buffer that will be rendered to.
   for (int n = 0; n < m_backBufferCount; n++)
   {
      DX::ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCommandAllocators[n].ReleaseAndGetAddressOf())));

      wchar_t name[25] = {};
      swprintf_s(name, L"Command Allocators %u", n);
      m_pCommandAllocators[n]->SetName(name);
   }

   // Create a command list for recording graphics commands.
   DX::ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(m_pCommandList.ReleaseAndGetAddressOf())));
   DX::ThrowIfFailed(m_pCommandList->Close());
   {
      static int count = -1;
      count += 1;
      wchar_t name[25] = {};
      swprintf_s(name, L"CommandList:%d", count);
      m_pCommandList->SetName(name);
   }

   const DXGI_FORMAT backBufferFormat = NoSRGB(targetFormatData.format);

   {
      // Create a descriptor for the swap chain.
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
      swapChainDesc.Width = m_width;
      swapChainDesc.Height = m_height;
      swapChainDesc.Format = backBufferFormat;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.BufferCount = m_backBufferCount;
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
      swapChainDesc.Flags = m_bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

      DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
      fsSwapChainDesc.Windowed = TRUE;

      // Create a swap chain for the window.
      Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
      DX::ThrowIfFailed(pDXGIFactory->CreateSwapChainForHwnd(
         pCommandQueue.Get(),
         hWnd,
         &swapChainDesc,
         &fsSwapChainDesc,
         nullptr,
         swapChain.GetAddressOf()
         ));

      DX::ThrowIfFailed(swapChain.As(&m_pSwapChain));

      // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
      DX::ThrowIfFailed(pDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
   }

   // Handle color space settings for HDR
   //UpdateColorSpace();

   // Obtain the back buffers for this window which will be the final render targets
   // and create render target views for each of them.
   //RenderTargetFormatData targetFormatData(_backBufferFormat, true, VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f));
   //RenderTargetDepthData targetDepthData(depthBufferFormat, true, 1.0f);
   for (int n = 0; n < m_backBufferCount; n++)
   {
      m_pRenderTargetBackBuffer[n] = std::make_unique<RenderTargetBackBuffer>(
         pDrawSystem,
         pDevice.Get(),
         n,
         targetFormatData,
         targetDepthData,
         m_pSwapChain.Get(),
         m_width,
         m_height
         );
   }

   // Reset the index to the current back buffer.
   m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

   {
      // Create a fence for tracking GPU execution progress.
      DX::ThrowIfFailed(pDevice->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf())));
      SetFenceValue(fenceValue + 1);
      m_pFence->SetName(L"Fence");
   }

   m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
   if (!m_fenceEvent.IsValid())
   {
      throw std::exception("CreateEvent");
   }
}

ScreenSizeResources::~ScreenSizeResources()
{
}

const UINT64 ScreenSizeResources::GetFenceValue()
{
   return m_fenceValues[m_backBufferIndex];
}

void ScreenSizeResources::SetFenceValue(const UINT64 value)
{
   m_fenceValues[m_backBufferIndex] = value;
   return;
}

void ScreenSizeResources::Prepare(
   ID3D12GraphicsCommandList*& pCommandList
   )
{
    // Reset command list and allocator.
    DX::ThrowIfFailed(m_pCommandAllocators[m_backBufferIndex]->Reset());
    DX::ThrowIfFailed(m_pCommandList->Reset(m_pCommandAllocators[m_backBufferIndex].Get(), nullptr));
    pCommandList = m_pCommandList.Get();

    return;
}

void ScreenSizeResources::WaitForGpu(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue) noexcept
{
   if (pCommandQueue && m_pFence && m_fenceEvent.IsValid())
   {
      // Schedule a Signal command in the GPU queue.
      UINT64 fenceValue = GetFenceValue();
      if (SUCCEEDED(pCommandQueue->Signal(m_pFence.Get(), fenceValue)))
      {
         // Wait until the Signal has been processed.
         if (SUCCEEDED(m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get())))
         {
            WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

            // Increment the fence value for the current frame.
            SetFenceValue(fenceValue + 1);
         }
      }
   }
   return;
}

const bool ScreenSizeResources::Present(
   HRESULT& hr,
   const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue
   )
{
   ((IRenderTarget*)m_pRenderTargetBackBuffer[m_backBufferIndex].get())->EndRender(m_pCommandList.Get());

    // Send the command list off to the GPU for processing.
    DX::ThrowIfFailed(m_pCommandList->Close());
    pCommandQueue->ExecuteCommandLists(1, CommandListCast(m_pCommandList.GetAddressOf()));

    if (m_bAllowTearing)
    {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        // Note this will fail if in true 'fullscreen' mode.
        hr = m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    }
    else
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        hr = m_pSwapChain->Present(1, 0);
    }

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        return false;
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
    return true;
}

IRenderTarget* ScreenSizeResources::GetRenderTargetBackBuffer()
{
   return m_pRenderTargetBackBuffer[m_backBufferIndex].get();
}

void ScreenSizeResources::UpdateBackBufferIndex()
{
   // Update the back buffer index.
   m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void ScreenSizeResources::MoveToNextFrame(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue)
{
   // Schedule a Signal command in the queue.
   const UINT64 currentFenceValue = GetFenceValue();
   DX::ThrowIfFailed(pCommandQueue->Signal(m_pFence.Get(), currentFenceValue));

   UpdateBackBufferIndex();
   const UINT64 nextBackBufferFenceValue = GetFenceValue();
   // If the next frame is not ready to be rendered yet, wait until it is ready.
   if (m_pFence->GetCompletedValue() < nextBackBufferFenceValue)
   {
      DX::ThrowIfFailed(m_pFence->SetEventOnCompletion(nextBackBufferFenceValue, m_fenceEvent.Get()));
      WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
   }

   // Set the fence value for the next frame.
   SetFenceValue(currentFenceValue + 1);
}
