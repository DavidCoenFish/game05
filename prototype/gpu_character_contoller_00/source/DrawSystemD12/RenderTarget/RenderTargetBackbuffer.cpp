#include "pch.h"

#include "DrawSystemD12/RenderTarget/RenderTargetBackbuffer.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/d3dx12.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapperItem.h"

RenderTargetBackBuffer::RenderTargetBackBuffer(
	DrawSystem* const pDrawSystem,
	ID3D12Device* const pDevice,
	const int bufferIndex,
	const RenderTargetFormatData& targetFormatData,
	const RenderTargetDepthData& targetDepthData,
	IDXGISwapChain* const pSwapChain,
	const int width,
	const int height
)
	: IRenderTarget()
	, m_bufferIndex(bufferIndex)
	, m_targetFormatData(targetFormatData)
	, m_targetClearValue(targetFormatData.MakeClearValue())
	, m_targetDepthData(targetDepthData)
	, m_depthClearValue(targetDepthData.MakeClearValue())
	, m_currentState(D3D12_RESOURCE_STATE_COMMON)
	, m_screenViewport{
	   0.0f,
	   0.0f,
	   static_cast<float>(width),
	   static_cast<float>(height),
	   D3D12_MIN_DEPTH,
	   D3D12_MAX_DEPTH
}
, m_scissorRect{
   0,
   0,
   width,
   height
}
, m_backBufferWidth(width)
, m_backBufferHeight(height)
{
	m_pRenderTargetDescriptor = pDrawSystem->MakeHeapWrapperRenderTargetView();
	if (DXGI_FORMAT_UNKNOWN != m_targetDepthData.format)
	{
		m_pDepthStencilViewDescriptor = pDrawSystem->MakeHeapWrapperDepthStencilView();
	}

	//back buffer render target view
	{
		DX::ThrowIfFailed(pSwapChain->GetBuffer(m_bufferIndex, IID_PPV_ARGS(m_pRenderTargetResource.GetAddressOf())));

		wchar_t name[32] = {};
		swprintf_s(name, L"Render target %u", m_bufferIndex);
		m_pRenderTargetResource->SetName(name);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_targetFormatData.format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		auto rtvDescriptor = m_pRenderTargetDescriptor->GetCPUHandle();
		pDevice->CreateRenderTargetView(m_pRenderTargetResource.Get(), &rtvDesc, rtvDescriptor);
	}

	//depth buffer
	if (DXGI_FORMAT_UNKNOWN != m_targetDepthData.format)
	{
		// Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
		// on this surface.
		CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			m_targetDepthData.format,
			width,
			height,
			1, // This depth stencil view has only one texture.
			0,  // Use a single mipmap level.
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
		depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		DX::ThrowIfFailed(pDevice->CreateCommittedResource(
			&depthHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&m_depthClearValue,
			IID_PPV_ARGS(m_pDepthResource.ReleaseAndGetAddressOf())
		));

		wchar_t name[32] = {};
		swprintf_s(name, L"Depth stencil %u", m_bufferIndex);
		m_pDepthResource->SetName(name);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = m_targetDepthData.format;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		auto handle = m_pDepthStencilViewDescriptor->GetCPUHandle();
		pDevice->CreateDepthStencilView(m_pDepthResource.Get(), &dsvDesc, handle);
	}

	return;
}

void RenderTargetBackBuffer::StartRender(ID3D12GraphicsCommandList* const pCommandList)
{
	if (m_currentState != D3D12_RESOURCE_STATE_RENDER_TARGET)
	{
		// Transition the render target into the correct state to allow for drawing into it.
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetResource.Get(), m_currentState, D3D12_RESOURCE_STATE_RENDER_TARGET);
		pCommandList->ResourceBarrier(1, &barrier);
		m_currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	pCommandList->RSSetViewports(1, &m_screenViewport);
	pCommandList->RSSetScissorRects(1, &m_scissorRect);

	auto pRenderTargetDescriptorHandle = m_pRenderTargetDescriptor->GetCPUHandle();

	if (m_pDepthStencilViewDescriptor)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE dsvDescriptor = m_pDepthStencilViewDescriptor->GetCPUHandle();
		pCommandList->OMSetRenderTargets(1, &pRenderTargetDescriptorHandle, FALSE, &dsvDescriptor);
	}
	else
	{
		pCommandList->OMSetRenderTargets(1, &pRenderTargetDescriptorHandle, FALSE, nullptr);
	}
	{
		PIXBeginEvent(pCommandList, PIX_COLOR_DEFAULT, L"RenderTargetBackBuffer::Clear");

		if ((nullptr != m_pRenderTargetDescriptor) && m_targetFormatData.clearOnSet)
		{
			auto descriptor = m_pRenderTargetDescriptor->GetCPUHandle();
			pCommandList->ClearRenderTargetView(
				descriptor,
				m_targetClearValue.Color,
				0,
				nullptr
			);
		}
		if ((nullptr != m_pDepthStencilViewDescriptor) && (m_targetDepthData.clearDepthOnSet || m_targetDepthData.clearStencilOnSet))
		{
			const D3D12_CLEAR_FLAGS flag = (D3D12_CLEAR_FLAGS)((m_targetDepthData.clearDepthOnSet ? D3D12_CLEAR_FLAG_DEPTH : 0) |
				(m_targetDepthData.clearStencilOnSet ? D3D12_CLEAR_FLAG_STENCIL : 0));

			auto descriptor = m_pDepthStencilViewDescriptor->GetCPUHandle();
			pCommandList->ClearDepthStencilView(
				descriptor,
				flag,
				m_depthClearValue.DepthStencil.Depth,
				m_depthClearValue.DepthStencil.Stencil,
				0,
				nullptr
			);
		}

		PIXEndEvent(pCommandList);
	}
}

void RenderTargetBackBuffer::EndRender(ID3D12GraphicsCommandList* const pCommandList)
{
	if (m_currentState != D3D12_RESOURCE_STATE_PRESENT)
	{
		// Transition the render target to the state that allows it to be presented to the display.
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetResource.Get(), m_currentState, D3D12_RESOURCE_STATE_PRESENT);
		pCommandList->ResourceBarrier(1, &barrier);
		m_currentState = D3D12_RESOURCE_STATE_PRESENT;
	}
}

void RenderTargetBackBuffer::GetFormatData(
	DXGI_FORMAT& depthFormat,
	int& renderTargetViewFormatCount,
	const DXGI_FORMAT*& pRenderTargetViewFormat
) const
{
	depthFormat = m_targetDepthData.format;
	renderTargetViewFormatCount = 1;
	pRenderTargetViewFormat = &m_targetFormatData.format;
}

const int RenderTargetBackBuffer::GetWidth() const
{
	return m_backBufferWidth;
}

const int RenderTargetBackBuffer::GetHeight() const
{
	return m_backBufferHeight;
}

