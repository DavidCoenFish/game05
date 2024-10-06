#include "common/common_pch.h"

#include "common/direct_xtk12/d3dx12.h"
#include "common/draw_system/render_target/render_target_backbuffer.h"
#include "common/draw_system/screen_size_resources.h"
#include "common/log/log.h"

static const DXGI_FORMAT NoSRGB(DXGI_FORMAT in_fmt) noexcept
{
	switch (in_fmt)
	{
	default:
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8X8_UNORM;
	}
	return in_fmt;
}

ScreenSizeResources::ScreenSizeResources(
	DrawSystem* const in_draw_system,
	const Microsoft::WRL::ComPtr<ID3D12Device>& in_device,
	const Microsoft::WRL::ComPtr<IDXGIFactory6>& in_dxgi_factory,
	const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& in_command_queue,
	const HWND in_hwnd,
	const UINT64 in_fence_value,
	const unsigned int in_back_buffer_count,
	const VectorInt2& in_size,
	const bool in_allow_tearing,
	const RenderTargetFormatData& in_target_format_data,
	const RenderTargetDepthData& in_target_depth_data
	) 
	: _back_buffer_count(in_back_buffer_count)
	, _back_buffer_index(0)
	, _allow_tearing(in_allow_tearing)
	, _size(
		std::max(in_size.GetX(), 1),
		std::max(in_size.GetY(), 1)
		)
{
	for (unsigned int index = 0; index < MAX_BACK_BUFFER_COUNT;++ index)
	{
		_fence_values[index] = in_fence_value;
	}
	// Create a command allocator for each back buffer that will be rendered to.
	for (int n = 0; n < _back_buffer_count; n++)
	{
		DX::ThrowIfFailed(in_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(_command_allocators[n].ReleaseAndGetAddressOf())
			));
		wchar_t name[25] = {};
		swprintf_s(
			name,
			L"Command Allocators %u",
			n
			);
		_command_allocators[n]->SetName(name);
	}
	// Create a command list for recording graphics commands.
	DX::ThrowIfFailed(in_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		_command_allocators[0].Get(),
		nullptr,
		IID_PPV_ARGS(_command_list.ReleaseAndGetAddressOf())
		));
	DX::ThrowIfFailed(_command_list->Close());

	{
		static int count = - 1;
		count += 1;
		wchar_t name[25] = {};
		swprintf_s(
			name,
			L"CommandList:%d",
			count
			);
		_command_list->SetName(name);
	}
	const DXGI_FORMAT back_buffer_format = NoSRGB(in_target_format_data._format);

	{
		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
		swap_chain_desc.Width = _size.GetX();
		swap_chain_desc.Height = _size.GetY();
		swap_chain_desc.Format = back_buffer_format;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.BufferCount = _back_buffer_count;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.Scaling = DXGI_SCALING_NONE; //DXGI_SCALING_STRETCH;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swap_chain_desc.Flags = _allow_tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
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
		DX::ThrowIfFailed(swap_chain.As(&_swap_chain));
		\
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
	for (int n = 0; n < _back_buffer_count; n++)
	{
		_render_target_back_buffer[n] = std::make_unique<RenderTargetBackBuffer>(
			in_draw_system,
			in_device.Get(),
			n,
			in_target_format_data,
			in_target_depth_data,
			_swap_chain.Get(),
			_size
			);
	}
	// Reset the index to the current back buffer.
	_back_buffer_index = _swap_chain->GetCurrentBackBufferIndex();

	{
		// Create a fence for tracking GPU execution progress.
		DX::ThrowIfFailed(in_device->CreateFence(
			in_fence_value,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf())
			));
		SetFenceValue(in_fence_value + 1);
		_fence->SetName(L"Fence");
	}
	_fence_event.Attach(CreateEventEx(
		nullptr,
		nullptr,
		0,
		EVENT_MODIFY_STATE | SYNCHRONIZE
		));
	if (!_fence_event.IsValid())
	{
		throw std::exception("CreateEvent");
	}

	//{
	//	DX::ThrowIfFailed(in_device->CreateFence(
	//		_frame_fence_value,
	//		D3D12_FENCE_FLAG_NONE,
	//		IID_PPV_ARGS(_frame_fence.ReleaseAndGetAddressOf())
	//		));
	//	_frame_fence->SetName(L"FrameFence");
	//}

	return;
}

ScreenSizeResources::~ScreenSizeResources(){}

const UINT64 ScreenSizeResources::GetFenceValue()
{
	return _fence_values[_back_buffer_index];
}

void ScreenSizeResources::SetFenceValue(const UINT64 in_value)
{
	_fence_values[_back_buffer_index] = in_value;
	return;
}

void ScreenSizeResources::Prepare(ID3D12GraphicsCommandList*&in_command_list)
{
	// Reset command list and allocator.
	DX::ThrowIfFailed(_command_allocators[_back_buffer_index]->Reset());
	DX::ThrowIfFailed(_command_list->Reset(
		_command_allocators[_back_buffer_index].Get(),
		nullptr
		));
	in_command_list = _command_list.Get();

	return;
}

void ScreenSizeResources::WaitForGpu(const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue) noexcept
{
	if (in_command_queue && _fence && _fence_event.IsValid())
	{
		// Schedule a Signal command in the GPU queue.
		UINT64 fence_value = GetFenceValue();
		if (SUCCEEDED(in_command_queue->Signal(
			_fence.Get(),
			fence_value
			)))
		{
			// Wait until the Signal has been processed.
			if (SUCCEEDED(_fence->SetEventOnCompletion(
				fence_value,
				_fence_event.Get()
				)))
			{
				WaitForSingleObjectEx(
					_fence_event.Get(),
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
	((IRenderTarget*) _render_target_back_buffer[_back_buffer_index].get())->EndRender(_command_list.Get());
	// Send the command list off to the GPU for processing.
	DX::ThrowIfFailed(_command_list->Close());
	in_command_queue->ExecuteCommandLists(
		1,
		CommandListCast(_command_list.GetAddressOf())
		);
	if (_allow_tearing)
	{
		// Recommended to always use tearing if supported when using a sync interval of 0.
		// Note this will fail if in true 'fullscreen' mode.
		in_hr = _swap_chain->Present(
			0,
			DXGI_PRESENT_ALLOW_TEARING
			);
	}
	else
	{
		// The first argument instructs DXGI to block until VSync, putting the application
		// To sleep until the next VSync. This ensures we don't waste any cycles rendering
		// Frames that will never be displayed to the screen.
		in_hr = _swap_chain->Present(
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

	//LOG_MESSAGE_RENDER("ScreenSizeResources Present");

	return true;
}

IRenderTarget* ScreenSizeResources::GetRenderTargetBackBuffer()
{
	return _render_target_back_buffer[_back_buffer_index].get();
}

void ScreenSizeResources::UpdateBackBufferIndex()
{
	// Update the back buffer index.
	_back_buffer_index = _swap_chain->GetCurrentBackBufferIndex();

	return;
}

void ScreenSizeResources::MoveToNextFrame(const Microsoft::WRL::ComPtr < ID3D12CommandQueue >&in_command_queue)
{
	// Schedule a Signal command in the queue.
	const UINT64 current_fence_value = GetFenceValue();
	DX::ThrowIfFailed(in_command_queue->Signal(
		_fence.Get(),
		current_fence_value
		));
	UpdateBackBufferIndex();
	const UINT64 next_back_buffer_fence_value = GetFenceValue();
	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (_fence->GetCompletedValue() < next_back_buffer_fence_value)
	{
		DX::ThrowIfFailed(_fence->SetEventOnCompletion(
			next_back_buffer_fence_value,
			_fence_event.Get()
			));
		WaitForSingleObjectEx(
			_fence_event.Get(),
			INFINITE,
			FALSE
			);
	}
	// Set the fence value for the next frame.
	SetFenceValue(current_fence_value + 1);

	return;
}

