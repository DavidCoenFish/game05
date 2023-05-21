#include "pch.h"

#include "DrawSystemD12/DeviceResources.h"
#include "DrawSystemD12/ScreenSizeResources.h"
#include "DrawSystemD12/d3dx12.h"
#include "DirectXTK12/GraphicsMemory.h"
#include "Log.h"
#include "Utf8.h"

DeviceResources::DeviceResources(
	const unsigned int backBufferCount,
	const D3D_FEATURE_LEVEL d3dFeatureLevel,
	const unsigned int options,
	const RenderTargetFormatData& targetFormatData,
	const RenderTargetDepthData& targetDepthData
)
	: m_options(options)
	, m_backBufferCount(backBufferCount)
	, m_dxgiFactoryFlags(0)
	, m_customCommandListFenceValue(0)
	, m_targetFormatData(targetFormatData)
	, m_targetDepthData(targetDepthData)
{
	static int sCount = -1;
	sCount += 1;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	//
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
		{
			debugController->EnableDebugLayer();
		}
		else
		{
			LOG_MESSAGE_WARNING("WARNING: Direct3D Debug Device is not available\n");
		}

		Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			m_dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

			DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
			{
				  80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
			};
			DXGI_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
		}
	}
#endif

	DX::ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf())));

	// Determines whether tearing support is available for fullscreen borderless windows.
	if (m_options & c_AllowTearing)
	{
		BOOL allowTearing = FALSE;
		HRESULT hr = m_pDXGIFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
		if (FAILED(hr) || !allowTearing)
		{
			m_options &= ~c_AllowTearing;

			LOG_MESSAGE_WARNING("WARNING: Variable refresh rate displays not supported");
		}
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	GetAdapter(adapter.GetAddressOf(), d3dFeatureLevel);

	// Create the DX12 API device object.
	HRESULT hr = D3D12CreateDevice(
		adapter.Get(),
		d3dFeatureLevel,
		IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf())
	);
	DX::ThrowIfFailed(hr);
	{
		wchar_t name[64] = {};
		swprintf_s(name, L"Device:%d", sCount);
		m_pDevice->SetName(name);
	}

#ifndef NDEBUG
	// Configure debug device (if active).
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
	if (SUCCEEDED(m_pDevice.As(&d3dInfoQueue)))
	{
#ifdef _DEBUG
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
		D3D12_MESSAGE_ID hide[] =
		{
		   D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		   D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
		   D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE
		};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif

	// Create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DX::ThrowIfFailed(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pCommandQueue.ReleaseAndGetAddressOf())));
	{
		wchar_t name[64] = {};
		swprintf_s(name, L"CommandQueue:%d", sCount);
		m_pCommandQueue->SetName(name);
	}

	//CreateWindowSizeDependentResources(pDrawSystem, hWnd);

	m_pGraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(m_pDevice.Get());
	if (nullptr == m_pGraphicsMemory)
	{
		throw std::exception("CreateEvent");
	}

	//Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCustomCommandAllocator;
	//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCustomCommandList;
	//Microsoft::WRL::ComPtr<ID3D12Fence> m_pCustomCommandFence;
	//Microsoft::WRL::Wrappers::Event m_customCommandFenceEvent;
	DX::ThrowIfFailed(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCustomCommandAllocator.ReleaseAndGetAddressOf())));
	{
		wchar_t name[64] = {};
		swprintf_s(name, L"CustomCommandAllocator:%d", sCount);
		m_pCustomCommandAllocator->SetName(name);
	}

	DX::ThrowIfFailed(m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCustomCommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_pCustomCommandList.ReleaseAndGetAddressOf())));
	{
		wchar_t name[64] = {};
		swprintf_s(name, L"CustomCommandList:%d", sCount);
		m_pCustomCommandList->SetName(name);
	}
	m_pCustomCommandList->Close();

	DX::ThrowIfFailed(m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pCustomCommandFence.ReleaseAndGetAddressOf())));
	m_customCommandFenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
	if (!m_customCommandFenceEvent.IsValid())
	{
		throw std::exception("CustomCommandFence");
	}

#if 1
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
	if (FAILED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
		|| (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5))
	{
		LOG_MESSAGE_WARNING("WARNING: Shader Model 6.5 is not supported");
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
	if (FAILED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features)))
		|| (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED))
	{
		LOG_MESSAGE_WARNING("WARNING: Mesh Shaders aren't supported");
	}

	{
		D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_FORMAT_SUPPORT1_RENDER_TARGET, D3D12_FORMAT_SUPPORT2_NONE };
		if (FAILED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))))
		{
			LOG_MESSAGE_WARNING("WARNING: formatSupport check failed DXGI_FORMAT_R32G32B32A32_FLOAT");
		}
	}

	{
		D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { DXGI_FORMAT_R32G32B32_FLOAT, D3D12_FORMAT_SUPPORT1_RENDER_TARGET, D3D12_FORMAT_SUPPORT2_NONE };
		if (FAILED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))))
		{
			LOG_MESSAGE_WARNING("WARNING: formatSupport check failed DXGI_FORMAT_R32G32B32_FLOAT");
		}
	}


#endif
}

DeviceResources::~DeviceResources()
{
	WaitForGpu(); //can go accross frames
	WaitForCustomCommand(); //make sure any outstanding command list are completed

	m_pCustomCommandAllocator.Reset();
	m_pCustomCommandList.Reset();
	m_pCustomCommandFence.Reset();

	m_pCommandQueue.Reset();
	m_pScreenSizeResources.reset();
	m_pGraphicsMemory.reset();

	m_pDevice.Reset();
	m_pDXGIFactory.Reset();

#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}

void DeviceResources::WaitForGpu() noexcept
{
	if (m_pScreenSizeResources)
	{
		m_pScreenSizeResources->WaitForGpu(m_pCommandQueue);
	}
	return;
}

void DeviceResources::WaitForCustomCommand() //make sure any outstanding command list are completed
{
	if (m_pCommandQueue && m_pCustomCommandFence && m_customCommandFenceEvent.IsValid())
	{
		if (SUCCEEDED(m_pCommandQueue->Signal(m_pCustomCommandFence.Get(), m_customCommandListFenceValue)))
		{
			// Wait until the Signal has been processed.
			if (SUCCEEDED(m_pCustomCommandFence->SetEventOnCompletion(m_customCommandListFenceValue, m_customCommandFenceEvent.Get())))
			{
				WaitForSingleObjectEx(m_customCommandFenceEvent.Get(), INFINITE, FALSE);

				// Increment the fence value for the current frame.
				m_customCommandListFenceValue += 1;
			}
		}
	}
}

const bool DeviceResources::OnResize(
	DrawSystem* const pDrawSystem, 
	const HWND hWnd, 
	int& out_width,
	int& out_height
	)
{
	return CreateWindowSizeDependentResources(pDrawSystem, hWnd, &out_width, &out_height);
}

const int DeviceResources::GetBackBufferIndex() const
{
	if (nullptr != m_pScreenSizeResources)
	{
		return m_pScreenSizeResources->GetBackBufferIndex();
	}
	return 0;
}

DirectX::GraphicsResource DeviceResources::AllocateUpload(const std::size_t size, void* const pDataOrNullptr, size_t alignment)
{
	const size_t alignedSize = (size + alignment - 1) & ~(alignment - 1);
	auto graphicsResource = m_pGraphicsMemory->Allocate(alignedSize, alignment);
	if (pDataOrNullptr)
	{
		memcpy(graphicsResource.Memory(), pDataOrNullptr, size);
	}
	return graphicsResource;
}

void DeviceResources::GetAdapter(IDXGIAdapter1** ppAdapter, const D3D_FEATURE_LEVEL d3dFeatureLevel)
{
	*ppAdapter = nullptr;

	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0;
		SUCCEEDED(m_pDXGIFactory->EnumAdapterByGpuPreference(
			adapterIndex,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())));
		adapterIndex++)
	{
		DXGI_ADAPTER_DESC1 desc;
		DX::ThrowIfFailed(adapter->GetDesc1(&desc));

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), d3dFeatureLevel, _uuidof(ID3D12Device), nullptr)))
		{
#ifdef _DEBUG
			LOG_MESSAGE("Direct3D Adapter (%u): VID:%04X, PID:%04X - %s", adapterIndex, desc.VendorId, desc.DeviceId, Utf8::Utf16ToUtf8(desc.Description).c_str());
#endif
			break;
		}
	}

#if !defined(NDEBUG)
	if (!adapter)
	{
		// Try WARP12 instead
		if (FAILED(m_pDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
		{
			throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
		}

		LOG_MESSAGE("Direct3D Adapter - WARP12");
	}
#endif

	if (!adapter)
	{
		throw std::exception("No Direct3D 12 device found");
	}

	*ppAdapter = adapter.Detach();
}

//return true if size changed
const bool DeviceResources::CreateWindowSizeDependentResources(
	DrawSystem* const pDrawSystem,
	const HWND hWnd,
	int* out_pWidth,
	int* out_pHeight
)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	const int width = rc.right - rc.left;
	const int height = rc.bottom - rc.top;

	if (nullptr != out_pWidth)
	{
		*out_pWidth = width;
	}
	if (nullptr != out_pHeight)
	{
		*out_pHeight = height;
	}

	//if we don't need to resize, then don't
	if ((nullptr != m_pScreenSizeResources) &&
		(width == m_pScreenSizeResources->GetWidth()) &&
		(height == m_pScreenSizeResources->GetHeight()))
	{
		return false;
	}

	WaitForGpu();

	UINT64 fenceValue = 0;
	if (nullptr != m_pScreenSizeResources)
	{
		fenceValue = m_pScreenSizeResources->GetFenceValue();
	}
	m_pScreenSizeResources.reset();

	const unsigned int swapFlag = (m_options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
	m_pScreenSizeResources = std::make_unique<ScreenSizeResources>(
		pDrawSystem,
		m_pDevice,
		m_pDXGIFactory,
		m_pCommandQueue,
		hWnd,
		fenceValue,
		m_backBufferCount,
		width,
		height,
		swapFlag,
		m_targetFormatData,
		m_targetDepthData
		);

	return true;
}

void DeviceResources::MoveToNextFrame()
{
	if (m_pScreenSizeResources)
	{
		m_pScreenSizeResources->MoveToNextFrame(m_pCommandQueue);
	}
}

void DeviceResources::Prepare(
	ID3D12GraphicsCommandList*& pCommandList
)
{
	if (nullptr != m_pScreenSizeResources)
	{
		m_pScreenSizeResources->Prepare(
			pCommandList
		);
	}
	return;
}
ID3D12Device2* const DeviceResources::GetD3dDevice()
{
	return m_pDevice.Get();
}
//ID3D12CommandQueue* const DeviceResources::GetCommandQueue()
//{
//   return m_pCommandQueue.Get();
//}

ID3D12GraphicsCommandList* DeviceResources::GetCustomCommandList()
{
	WaitForCustomCommand();
	DX::ThrowIfFailed(m_pCustomCommandAllocator->Reset());
	DX::ThrowIfFailed(m_pCustomCommandList->Reset(m_pCustomCommandAllocator.Get(), nullptr));

	DX::ThrowIfFailed(m_pCommandQueue->Signal(m_pCustomCommandFence.Get(), m_customCommandListFenceValue));

	if (m_pCustomCommandFence->GetCompletedValue() < m_customCommandListFenceValue)
	{
		DX::ThrowIfFailed(m_pCustomCommandFence->SetEventOnCompletion(m_customCommandListFenceValue, m_customCommandFenceEvent.Get()));
		WaitForSingleObjectEx(m_customCommandFenceEvent.Get(), INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	m_customCommandListFenceValue += 1;

	return m_pCustomCommandList.Get();
}

void DeviceResources::CustomCommandListFinish(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->Close();
	m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(&pCommandList));
}

//void DeviceResources::Clear()
//{
//   if (nullptr == m_pScreenSizeResources)
//   {
//      return;
//   }
//   m_pScreenSizeResources->Clear();
//}

const bool DeviceResources::Present()
{
	if (nullptr == m_pScreenSizeResources)
	{
		return false;
	}
	HRESULT hr = SEVERITY_SUCCESS;
	if (false == m_pScreenSizeResources->Present(hr, m_pCommandQueue))
	{
#ifdef _DEBUG
		char buff[64] = {};
		sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
			static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pDevice->GetDeviceRemovedReason() : hr));
		OutputDebugStringA(buff);
#endif

		return false;
	}

	MoveToNextFrame();

	if (!m_pDXGIFactory->IsCurrent())
	{
		// Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
		DX::ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf())));
	}

	if (m_pGraphicsMemory)
	{
		m_pGraphicsMemory->Commit(m_pCommandQueue.Get());
	}

	return true;
}

IRenderTarget* DeviceResources::GetRenderTargetBackBuffer()
{
	if (nullptr != m_pScreenSizeResources)
	{
		return m_pScreenSizeResources->GetRenderTargetBackBuffer();
	}
	return nullptr;
}

