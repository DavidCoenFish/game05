#include "CommonPCH.h"

#include "Common/DrawSystem/RenderTarget/RenderTargetTexture.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/HeapWrapper/HeapWrapperItem.h"
#include "Common/DrawSystem/d3dx12.h"

RenderTargetTexture::Resource::Resource(
	const D3D12_CLEAR_VALUE& clearValue,
	const DXGI_FORMAT format,
	const Microsoft::WRL::ComPtr<ID3D12Resource>& renderTarget,
	const std::shared_ptr<HeapWrapperItem>& pRenderTargetViewDescriptor,
	const std::shared_ptr<HeapWrapperItem>& pShaderResourceViewDescriptor,
	const bool bClearColor,
	const bool bClearDepth,
	const bool bClearStencil
)
	: m_format(format)
	, m_renderTarget(renderTarget)
	, m_pRenderTargetViewDescriptor(pRenderTargetViewDescriptor)
	, m_pShaderResourceViewDescriptor(pShaderResourceViewDescriptor)
	, m_clearValue{ clearValue }
	, m_bClearColor(bClearColor)
	, m_bClearDepth(bClearDepth)
	, m_bClearStencil(bClearStencil)
{
	return;
}

RenderTargetTexture::RenderTargetTexture(
	DrawSystem* const pDrawSystem,
	const std::vector< RenderTargetFormatData >& targetFormatDataArray,
	const RenderTargetDepthData& targetDepthData,
	const int width,
	const int height,
	const bool bResizeWithScreen
)
	: IRenderTarget()
	, IResource(pDrawSystem)
	, m_width(width)
	, m_height(height)
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
, m_currentStateRenderTarget(D3D12_RESOURCE_STATE_COMMON)
, m_currentStateDepthResource(D3D12_RESOURCE_STATE_COMMON)
, m_targetFormatArray()
, m_bResizeWithScreen(bResizeWithScreen)
{
	for (const auto& iter : targetFormatDataArray)
	{
		D3D12_CLEAR_VALUE clearValue = iter.MakeClearValue();
		m_targetResourceArray.push_back(std::make_shared< Resource >(
			clearValue,
			iter.format,
			nullptr,
			pDrawSystem->MakeHeapWrapperRenderTargetView(),
			pDrawSystem->MakeHeapWrapperCbvSrvUav(),
			iter.clearOnSet
			));
		m_targetFormatArray.push_back(iter.format);
	}
	if (targetDepthData.format != DXGI_FORMAT_UNKNOWN)
	{
		D3D12_CLEAR_VALUE clearValue = targetDepthData.MakeClearValue();
		m_pDepthResource = std::make_shared< Resource >(
			clearValue,
			targetDepthData.format,
			nullptr,
			pDrawSystem->MakeHeapWrapperDepthStencilView(),
			targetDepthData.shaderResource ? pDrawSystem->MakeHeapWrapperCbvSrvUav() : nullptr,
			false,
			targetDepthData.clearDepth,
			targetDepthData.clearStencil
			);
	}

	return;
}

RenderTargetTexture::~RenderTargetTexture()
{
	return;
}


std::shared_ptr< HeapWrapperItem > RenderTargetTexture::GetShaderResourceHeapWrapperItem(const int index) const
{
	if ((0 <= index) && (index < (int)m_targetResourceArray.size()))
	{
		return m_targetResourceArray[index]->m_pShaderResourceViewDescriptor;
	}
	return nullptr;
}

std::shared_ptr< HeapWrapperItem > RenderTargetTexture::GetDepthResourceHeapWrapperItem() const
{
	if (m_pDepthResource)
	{
		return m_pDepthResource->m_pRenderTargetViewDescriptor;
	}
	return nullptr;
}

std::shared_ptr< HeapWrapperItem > RenderTargetTexture::GetDepthShaderResourceHeapWrapperItem() const
{
	if (m_pDepthResource)
	{
		return m_pDepthResource->m_pShaderResourceViewDescriptor;
	}

	return nullptr;
}

void RenderTargetTexture::Resize(
	ID3D12GraphicsCommandList* const pCommandList,
	ID3D12Device2* const pDevice,
	const int width,
	const int height
)
{
	if ((m_width == width) &&
		(m_height == height))
	{
		return;
	}

	OnDeviceLost();

	m_width = width;
	m_height = height;
	m_scissorRect.right = width;
	m_scissorRect.bottom = height;
	m_screenViewport.Width = (float)width;
	m_screenViewport.Height = (float)height;

	OnDeviceRestored(pCommandList, pDevice);
}

const int RenderTargetTexture::GetWidth() const
{
	return m_width;
}

const int RenderTargetTexture::GetHeight() const
{
	return m_height;
}

void RenderTargetTexture::OnDeviceLost()
{
	for (auto iter : m_targetResourceArray)
	{
		iter->m_renderTarget.Reset();
	}
	if (nullptr != m_pDepthResource)
	{
		m_pDepthResource->m_renderTarget.Reset();
	}
}

void RenderTargetTexture::OnDeviceRestored(
	ID3D12GraphicsCommandList* const,
	ID3D12Device2* const pDevice
)
{
	//so, is this a good idea, to allow Resize(), call OnDeviceLost
	//OnDeviceLost();

	m_arrayRenderTargetDescriptors.clear();
	m_currentStateRenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET;
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	for (auto iter : m_targetResourceArray)
	{
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			iter->m_format,
			static_cast<UINT64>(m_width),
			static_cast<UINT>(m_height),
			1,
			1,
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);
		DX::ThrowIfFailed(
			pDevice->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
				&desc,
				m_currentStateRenderTarget,
				&iter->m_clearValue,
				IID_PPV_ARGS(iter->m_renderTarget.ReleaseAndGetAddressOf())
			)
		);
		iter->m_renderTarget->SetName(L"RenderTargetResource");
		auto renderTargetViewDescriptor = iter->m_pRenderTargetViewDescriptor->GetCPUHandle();
		m_arrayRenderTargetDescriptors.push_back(renderTargetViewDescriptor);
		pDevice->CreateRenderTargetView(
			iter->m_renderTarget.Get(),
			nullptr,
			renderTargetViewDescriptor
		);
		const int frameCount = m_pDrawSystem->GetBackBufferCount();
		for (int index = 0; index < frameCount; ++index)
		{
			pDevice->CreateShaderResourceView(
				iter->m_renderTarget.Get(),
				nullptr,
				iter->m_pShaderResourceViewDescriptor->GetCPUHandleFrame(index)
			);
		}
	}

	m_currentStateDepthResource = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	if (nullptr != m_pDepthResource)
	{
		D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			m_pDepthResource->m_format,
			m_width,
			m_height,
			1, // This depth stencil view has only one texture.
			1,  // Use a single mipmap level.
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

		DX::ThrowIfFailed(pDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			m_currentStateDepthResource,
			&m_pDepthResource->m_clearValue,
			IID_PPV_ARGS(m_pDepthResource->m_renderTarget.ReleaseAndGetAddressOf())
		));

		m_pDepthResource->m_renderTarget->SetName(L"RenderTarget Depth Stencil");

		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = m_pDepthResource->m_format;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			pDevice->CreateDepthStencilView(m_pDepthResource->m_renderTarget.Get(), &dsvDesc, m_pDepthResource->m_pRenderTargetViewDescriptor->GetCPUHandle());
		}

		if (nullptr != m_pDepthResource->m_pShaderResourceViewDescriptor)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			const int frameCount = m_pDrawSystem->GetBackBufferCount();
			for (int index = 0; index < frameCount; ++index)
			{
				pDevice->CreateShaderResourceView(
					m_pDepthResource->m_renderTarget.Get(),
					&srvDesc,
					m_pDepthResource->m_pShaderResourceViewDescriptor->GetCPUHandleFrame(index)
				);
			}
		}
	}
	return;
}

void RenderTargetTexture::OnResize(
	ID3D12GraphicsCommandList* const pCommandList,
	ID3D12Device2* const pDevice,
	const int screenWidth,
	const int screenHeight
)
{
	if (false == m_bResizeWithScreen)
	{
		return;
	}

	Resize(
		pCommandList,
		pDevice,
		screenWidth,
		screenHeight
	);

	return;
}

void RenderTargetTexture::StartRender(ID3D12GraphicsCommandList* const pCommandList)
{
	TransitionResource(pCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	pCommandList->RSSetViewports(1, &m_screenViewport);
	pCommandList->RSSetScissorRects(1, &m_scissorRect);

	UINT NumRenderTargetDescriptors = 0;
	const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors = nullptr;
	const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor = nullptr;

	if (0 < m_arrayRenderTargetDescriptors.size())
	{
		NumRenderTargetDescriptors = (UINT)m_arrayRenderTargetDescriptors.size();
		pRenderTargetDescriptors = m_arrayRenderTargetDescriptors.data();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE dsvDescriptor = m_pDepthResource ? m_pDepthResource->m_pRenderTargetViewDescriptor->GetCPUHandle() : D3D12_CPU_DESCRIPTOR_HANDLE();
	if (m_pDepthResource)
	{
		pDepthStencilDescriptor = &dsvDescriptor;
	}

	pCommandList->OMSetRenderTargets(
		NumRenderTargetDescriptors,
		pRenderTargetDescriptors,
		FALSE,
		pDepthStencilDescriptor
	);

	{
		PIXBeginEvent(pCommandList, PIX_COLOR_DEFAULT, L"RenderTargetTexture::StartRender::Clear");

		for (const auto& iter : m_targetResourceArray)
		{
			if (false == iter->m_bClearColor)
			{
				continue;
			}
			auto descriptor = iter->m_pRenderTargetViewDescriptor->GetCPUHandle();
			pCommandList->ClearRenderTargetView(
				descriptor,
				iter->m_clearValue.Color,
				0,
				nullptr
			);
		}
		if (m_pDepthResource && (m_pDepthResource->m_bClearDepth || m_pDepthResource->m_bClearStencil))
		{
			const D3D12_CLEAR_FLAGS flag = (D3D12_CLEAR_FLAGS)((m_pDepthResource->m_bClearDepth ? D3D12_CLEAR_FLAG_DEPTH : 0) |
				(m_pDepthResource->m_bClearStencil ? D3D12_CLEAR_FLAG_STENCIL : 0));

			auto descriptor = m_pDepthResource->m_pRenderTargetViewDescriptor->GetCPUHandle();
			pCommandList->ClearDepthStencilView(
				descriptor,
				flag,
				m_pDepthResource->m_clearValue.DepthStencil.Depth,
				m_pDepthResource->m_clearValue.DepthStencil.Stencil,
				0,
				nullptr
			);
		}

		PIXEndEvent(pCommandList);
	}

	return;
}

void RenderTargetTexture::EndRender(ID3D12GraphicsCommandList* const pCommandList)
{
	TransitionResource(pCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_READ);
	return;
}

void RenderTargetTexture::TransitionResource(
	ID3D12GraphicsCommandList* const pCommandList,
	const D3D12_RESOURCE_STATES newStateRenderTarget,
	const D3D12_RESOURCE_STATES newStateDepthResource
)
{
	if (m_currentStateRenderTarget != newStateRenderTarget)
	{
		for (const auto& iter : m_targetResourceArray)
		{
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				iter->m_renderTarget.Get(),
				m_currentStateRenderTarget,
				newStateRenderTarget
			);
			pCommandList->ResourceBarrier(1, &barrier);
		}
		m_currentStateRenderTarget = newStateRenderTarget;
	}
	if (m_currentStateDepthResource != newStateDepthResource)
	{
		if (m_pDepthResource)
		{
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				m_pDepthResource->m_renderTarget.Get(),
				m_currentStateDepthResource,
				newStateDepthResource
			);
			pCommandList->ResourceBarrier(1, &barrier);
		}
		m_currentStateDepthResource = newStateDepthResource;
	}
}

void RenderTargetTexture::GetFormatData(
	DXGI_FORMAT& depthFormat,
	int& renderTargetViewFormatCount,
	const DXGI_FORMAT*& pRenderTargetViewFormat
) const
{
	depthFormat = m_pDepthResource ? m_pDepthResource->m_format : DXGI_FORMAT_UNKNOWN;
	renderTargetViewFormatCount = (int)m_targetFormatArray.size();
	pRenderTargetViewFormat = (DXGI_FORMAT*)(m_targetFormatArray.data());
	return;
}
