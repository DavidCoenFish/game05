#include "pch.h"

#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/DeviceResources.h"
#include "DrawSystemD12/DrawSystemFrame.h"
#include "DrawSystemD12/CustomCommandList.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapper.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapperItem.h"
#include "DrawSystemD12/Shader/Shader.h"
#include "DrawSystemD12/Shader/ShaderResource.h"
#include "DrawSystemD12/Shader/UnorderedAccess.h"
#include "DrawSystemD12/Shader/UnorderedAccessInfo.h"
#include "DrawSystemD12/RenderTarget/RenderTargetTexture.h"
#include "DrawSystemD12/Geometry/GeometryGeneric.h"
#include "DirectXTK12/GraphicsMemory.h"
//#include "JSON/JSONDrawSystem.h"

std::unique_ptr< DrawSystem > DrawSystem::Factory(const HWND hWnd)
{
	return std::make_unique< DrawSystem >(
		hWnd
		);
}

DrawSystem::DrawSystem(
	const HWND hWnd,
	const unsigned int backBufferCount,
	const D3D_FEATURE_LEVEL d3dFeatureLevel,
	const unsigned int options,
	const RenderTargetFormatData& targetFormatData,
	const RenderTargetDepthData& targetDepthData
)
	: m_hWnd(hWnd)
	, m_backBufferCount(backBufferCount)
	, m_d3dFeatureLevel(d3dFeatureLevel)
	, m_options(options)
	, m_targetFormatData(targetFormatData)
	, m_targetDepthData(targetDepthData)
{
	m_pHeapWrapperCbvSrvUav = std::make_shared< HeapWrapper >(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, backBufferCount, true);
	m_pHeapWrapperSampler = std::make_shared< HeapWrapper >(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	m_pHeapWrapperRenderTargetView = std::make_shared< HeapWrapper >(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_pHeapWrapperDepthStencilView = std::make_shared< HeapWrapper >(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	CreateDeviceResources();
}

DrawSystem::~DrawSystem()
{
	WaitForGpu();
	for (auto pIter : m_listResource)
	{
		pIter->OnDeviceLost();
	}
	m_listResource.clear();
	m_pDeviceResources.reset();
}

DirectX::GraphicsResource DrawSystem::AllocateConstant(const std::size_t size, void* const pConstants)
{
	return AllocateUpload(size, pConstants, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

DirectX::GraphicsResource DrawSystem::AllocateUpload(
	const std::size_t size,
	void* const pDataOrNullptr,
	size_t alignment
)
{
	if (m_pDeviceResources)
	{
		return m_pDeviceResources->AllocateUpload(size, pDataOrNullptr, alignment);
	}
	return DirectX::GraphicsResource();
}

std::shared_ptr< Shader > DrawSystem::MakeShader(
	ID3D12GraphicsCommandList* const pCommandList,
	const ShaderPipelineStateData& pipelineStateData,
	const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
	const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
	const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
	const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo,
	const std::vector< std::shared_ptr< ConstantBufferInfo > >& arrayShaderConstantsInfo,
	const std::shared_ptr< std::vector<uint8_t> >& pComputeShaderData,
	const std::vector< std::shared_ptr< UnorderedAccessInfo > >& arrayUnorderedAccessInfo
)
{
	auto pResult = std::make_shared<Shader>(
		this,
		pipelineStateData,
		pVertexShaderData,
		pGeometryShaderData,
		pPixelShaderData,
		arrayShaderResourceInfo,
		arrayShaderConstantsInfo,
		pComputeShaderData,
		arrayUnorderedAccessInfo
		);
	if (pResult && m_pDeviceResources)
	{
		((IResource*)(pResult.get()))->OnDeviceRestored(
			pCommandList,
			m_pDeviceResources->GetD3dDevice()
		);
	}
	return pResult;
}

std::shared_ptr< GeometryGeneric > DrawSystem::MakeGeometryGeneric(
	ID3D12GraphicsCommandList* const pCommandList,
	const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
	const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
	const std::vector< float >& vertexDataRaw,
	const int floatPerVertex
)
{
	auto pResult = std::make_shared<GeometryGeneric>(
		this,
		primitiveTopology,
		inputElementDescArray,
		vertexDataRaw,
		floatPerVertex
		);
	if (pResult && m_pDeviceResources)
	{
		((IResource*)(pResult.get()))->OnDeviceRestored(
			pCommandList,
			m_pDeviceResources->GetD3dDevice()
		);
	}
	return pResult;
}

std::shared_ptr< ShaderResource > DrawSystem::MakeShaderResource(
	ID3D12GraphicsCommandList* const pCommandList,
	const std::shared_ptr< HeapWrapperItem >& shaderResource,
	const D3D12_RESOURCE_DESC& desc,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
	const std::vector<uint8_t>& data
)
{
	auto pResult = std::make_shared<ShaderResource>(
		this,
		shaderResource,
		desc,
		shaderResourceViewDesc,
		data
		);
	if (pResult && m_pDeviceResources)
	{
		((IResource*)(pResult.get()))->OnDeviceRestored(
			pCommandList,
			m_pDeviceResources->GetD3dDevice()
		);
	}
	return pResult;
}

std::shared_ptr< UnorderedAccess > DrawSystem::MakeUnorderedAccess(
	ID3D12GraphicsCommandList* const pCommandList,
	const D3D12_RESOURCE_DESC& desc,
	const D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessViewDesc,
	const bool makeShaderViewHeapWrapperItem
	)
{
	auto pHeapWrapperItem = MakeHeapWrapperCbvSrvUav();
	auto pShaderViewHeapWrapperOrNull = makeShaderViewHeapWrapperItem ? MakeHeapWrapperCbvSrvUav() : nullptr;
	auto pResult = std::make_shared<UnorderedAccess>(
		this,
		pHeapWrapperItem,
		desc,
		unorderedAccessViewDesc,
		pShaderViewHeapWrapperOrNull
		);
	if (pResult && m_pDeviceResources)
	{
		((IResource*)(pResult.get()))->OnDeviceRestored(
			pCommandList,
			m_pDeviceResources->GetD3dDevice()
		);
	}
	return pResult;
}

std::shared_ptr< RenderTargetTexture > DrawSystem::MakeRenderTargetTexture(
	ID3D12GraphicsCommandList* const pCommandList,
	const std::vector< RenderTargetFormatData >& targetFormatDataArray,
	const RenderTargetDepthData& targetDepthData,
	const int width,
	const int height,
	const bool bResizeWithScreen
)
{
	auto pResult = std::make_shared<RenderTargetTexture>(
		this,
		targetFormatDataArray,
		targetDepthData,
		width,
		height,
		bResizeWithScreen
		);
	if (pResult && m_pDeviceResources)
	{
		((IResource*)(pResult.get()))->OnDeviceRestored(
			pCommandList,
			m_pDeviceResources->GetD3dDevice()
		);
	}
	return pResult;
}

void DrawSystem::ResizeRenderTargetTexture(
	std::shared_ptr< RenderTargetTexture >& pRenderTargetTexture,
    ID3D12GraphicsCommandList* const pCommandList,
    const int width,
    const int height
    )
{
	if ((nullptr != m_pDeviceResources) && (nullptr != pRenderTargetTexture))
	{
		pRenderTargetTexture->Resize(
			pCommandList,
			m_pDeviceResources->GetD3dDevice(),
			width,
			height
			);
	}
}

std::shared_ptr<CustomCommandList> DrawSystem::CreateCustomCommandList(
	ID3D12PipelineState* const pPipelineStateObjectOrNull
	)
{
	if (m_pDeviceResources)
	{
		auto pCommandList = m_pDeviceResources->GetCustomCommandList(pPipelineStateObjectOrNull);
		return std::make_shared< CustomCommandList>(
			*this,
			pCommandList
			);
	}
	return nullptr;
}

void DrawSystem::CustomCommandListFinish(ID3D12GraphicsCommandList* pCommandList)
{
	if (m_pDeviceResources)
	{
		m_pDeviceResources->CustomCommandListFinish(pCommandList);
	}
	return;
}

std::unique_ptr< DrawSystemFrame > DrawSystem::CreateNewFrame()
{
	return std::make_unique< DrawSystemFrame >(*this);
}

void DrawSystem::Prepare(
	ID3D12GraphicsCommandList*& pCommandList
)
{
	if (nullptr != m_pDeviceResources)
	{
		m_pDeviceResources->Prepare(
			pCommandList
		);
	}

	return;
}

void DrawSystem::Present()
{
	if (nullptr == m_pDeviceResources)
	{
		return;
	}

	if (false == m_pDeviceResources->Present())
	{
		CreateDeviceResources();
	}
}

IRenderTarget* DrawSystem::GetRenderTargetBackBuffer()
{
	if (m_pDeviceResources)
	{
		return m_pDeviceResources->GetRenderTargetBackBuffer();
	}
	return nullptr;
}

std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperCbvSrvUav(const int length)
{
	return HeapWrapperItem::Factory(
		m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
		m_pHeapWrapperCbvSrvUav,
		length
	);
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperSampler(const int length)
{
	return HeapWrapperItem::Factory(
		m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
		m_pHeapWrapperSampler,
		length
	);
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperRenderTargetView(const int length)
{
	return HeapWrapperItem::Factory(
		m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
		m_pHeapWrapperRenderTargetView,
		length
	);
}
std::shared_ptr<HeapWrapperItem> DrawSystem::MakeHeapWrapperDepthStencilView(const int length)
{
	return HeapWrapperItem::Factory(
		m_pDeviceResources ? m_pDeviceResources->GetD3dDevice() : nullptr,
		m_pHeapWrapperDepthStencilView,
		length
	);
}

void DrawSystem::WaitForGpu() noexcept
{
	if (m_pDeviceResources)
	{
		m_pDeviceResources->WaitForGpu();
	}
}

void DrawSystem::OnResize()
{
	bool bResized = false;
	int width = 0;
	int height = 0;
	if (m_pDeviceResources)
	{
		bResized = m_pDeviceResources->OnResize(this, m_hWnd, width, height);
	}

	if ((true == bResized) && (0 < m_listResource.size()))
	{
		auto pCommandList = CreateCustomCommandList();
		for (auto pIter : m_listResource)
		{
			pIter->OnResize(
				pCommandList->GetCommandList(),
				m_pDeviceResources->GetD3dDevice(),
				width, 
				height
			);
		}
	}
}

void DrawSystem::AddResource(IResource* const pResource)
{
	if (pResource)
	{
		m_listResource.push_back(pResource);
	}
}

void DrawSystem::RemoveResource(IResource* const pResource)
{
	//if RemoveResource is called by the dtor, then not a good idea to call a virutal func on the pResource?
	//if (pResource)
	//{
	//   pResource->OnDeviceLost();
	//}

	m_listResource.remove(pResource);
}

const int DrawSystem::GetBackBufferIndex() const
{
	if (nullptr != m_pDeviceResources)
	{
		m_pDeviceResources->GetBackBufferIndex();
	}
	return 0;
}


void DrawSystem::CreateDeviceResources()
{
	for (auto pIter : m_listResource)
	{
		pIter->OnDeviceLost();
	}
	m_pDeviceResources.reset();
	m_pDeviceResources = std::make_unique< DeviceResources >(
		2,
		m_d3dFeatureLevel,
		m_options,
		m_targetFormatData,
		m_targetDepthData
		);
	//two pass construction as rendertargetbackbuffer calls MakeHeapWrapperRenderTargetView, MakeHeapWrapperDepthStencilView which need m_pDeviceResources assigned
	m_pDeviceResources->CreateWindowSizeDependentResources(this, m_hWnd);

	if (0 < m_listResource.size())
	{
		auto pCommandList = CreateCustomCommandList();
		for (auto pIter : m_listResource)
		{
			pIter->OnDeviceRestored(
				pCommandList->GetCommandList(),
				m_pDeviceResources->GetD3dDevice()
			);
		}
	}
}


