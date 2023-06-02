#include "pch.h"
#include "ApplicationCompute.h"
#include "Log.h"
#include "FileSystem.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/Shader/Shader.h"
#include "DrawSystemD12/Shader/ShaderResource.h"
#include "DrawSystemD12/Shader/ShaderResourceInfo.h"
#include "DrawSystemD12/Shader/UnorderedAccess.h"
#include "DrawSystemD12/Shader/UnorderedAccessInfo.h"
#include "DrawSystemD12/RenderTarget/RenderTargetTexture.h"
#include "DrawSystemD12/Geometry/GeometryGeneric.h"
#include "DrawSystemD12/DrawSystemFrame.h"
#include "DrawSystemD12/CustomCommandList.h"

IApplication* const ApplicationCompute::Factory(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
{
	return new ApplicationCompute(
		hWnd,
		bFullScreen,
		defaultWidth,
		defaultHeight
		);
}

ApplicationCompute::ApplicationCompute(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
	: IApplication(hWnd, bFullScreen, defaultWidth, defaultHeight)
{
	LOG_MESSAGE("ApplicationCompute  ctor %p", this);

	m_pDrawSystem = std::make_unique< DrawSystem>(
		hWnd,
		2, //const unsigned int backBufferCount,
		D3D_FEATURE_LEVEL_11_0, //const D3D_FEATURE_LEVEL d3dFeatureLevel,
		0, //const unsigned int options
		RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM, true, VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f))//const RenderTargetFormatData& targetFormatData = RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM),
		);

	m_constantBuffer = ACConstantBuffer({
		{static_cast<float>(defaultWidth), static_cast<float>(defaultHeight), 10.0f, 0.0f}, 
		{4.0f, 2.25f, -0.65f, 0.0f}});

	auto pCommandList = m_pDrawSystem->CreateCustomCommandList();


	//compute render target (UnorderedAccess)
	{
		auto pHeapWrapperItem = m_pDrawSystem->MakeHeapWrapperCbvSrvUav();
		auto pShaderHeapWrapperItem = m_pDrawSystem->MakeHeapWrapperCbvSrvUav();
		std::vector< RenderTargetFormatData > targetFormatDataArray;
		targetFormatDataArray.push_back(RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM));

		const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 1920, 1080, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		D3D12_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc({});
		unorderedAccessViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		unorderedAccessViewDesc.Texture2D.MipSlice = 0;
		unorderedAccessViewDesc.Texture2D.PlaneSlice = 0;

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		//shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//shaderResourceViewDesc.

		//D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
		m_pComputeOutputTexture = m_pDrawSystem->MakeUnorderedAccess(
			pCommandList->GetCommandList(),
			pHeapWrapperItem, //const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
			desc, //const D3D12_RESOURCE_DESC& desc, 
			unorderedAccessViewDesc,//const D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessViewDesc
			pShaderHeapWrapperItem,
			shaderResourceViewDesc
			);
	}

	//compute shader
	{
		auto pComputeShaderData = FileSystem::SyncReadFile("ComputeShader.cso");
		ShaderPipelineStateData computePipelineStateData = ShaderPipelineStateData::FactoryComputeShader();

		std::vector< std::shared_ptr< ShaderResourceInfo > > arrayShaderResourceInfo;
		//arrayShaderResourceInfo.push_back( ShaderResourceInfo::FactorySampler( m_pComputeOutputTexture->GetShaderResourceHeapWrapperItem(), D3D12_SHADER_VISIBILITY_PIXEL ) );

		std::vector< std::shared_ptr< ConstantBufferInfo > > arrayShaderConstantsInfo;
		arrayShaderConstantsInfo.push_back( ConstantBufferInfo::Factory(m_constantBuffer));
			
		std::vector< std::shared_ptr< UnorderedAccessInfo > > arrayUnorderedAccessInfo;
		arrayUnorderedAccessInfo.push_back( std::make_shared< UnorderedAccessInfo>(
			m_pComputeOutputTexture->GetHeapWrapperItem()
			));

		m_pComputeShader = m_pDrawSystem->MakeShader(
			pCommandList->GetCommandList(),
			computePipelineStateData,
			nullptr,
			nullptr,
			nullptr,
			arrayShaderResourceInfo,
			arrayShaderConstantsInfo,
			pComputeShaderData,
			arrayUnorderedAccessInfo
			);
	}

	std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray({
		D3D12_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0 // UINT InstanceDataStepRate;
			},
		D3D12_INPUT_ELEMENT_DESC{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0 // UINT InstanceDataStepRate;
			}
		});

	// Geometry
	{
		m_pGeometry = m_pDrawSystem->MakeGeometryGeneric(
			pCommandList->GetCommandList(),
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			inputElementDescArray,
			//std::vector<float>({
			//	0.0f,0.5f, 1.0f, 0.5f, 
			//	0.5f,-0.5f, 0.0f, 1.0f,
			//	-0.5f,-0.5f, 0.0f, 0.0f
			//	}),
			std::vector<float>({
				-1.0f,1.0f, 0.0f, 1.0f, 
				1.0f,1.0f, 1.0f, 1.0f,
				-1.0f,-1.0f, 0.0f, 0.0f,
				1.0f,-1.0f, 1.0f, 0.0f
				}),
			4);
	}

	// Present Shader
	{
		auto pVertexShaderData = FileSystem::SyncReadFile("TextureVertexShader.cso");
		auto pPixelShaderData = FileSystem::SyncReadFile("TexturePixelShader.cso");
		std::vector<DXGI_FORMAT> renderTargetFormat;
		renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

		std::vector< std::shared_ptr< ShaderResourceInfo > > arrayTexture;
		arrayTexture.push_back( ShaderResourceInfo::FactorySampler( m_pComputeOutputTexture->GetShaderViewHeapWrapperItem(), D3D12_SHADER_VISIBILITY_PIXEL ) );

		ShaderPipelineStateData shaderPipelineStateData(
			inputElementDescArray,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
			renderTargetFormat,
			CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
			);

		m_pPresentShader = m_pDrawSystem->MakeShader(
			pCommandList->GetCommandList(),
			shaderPipelineStateData,
			pVertexShaderData,
			nullptr,
			pPixelShaderData,
			arrayTexture
			);
	}
}

ApplicationCompute ::~ApplicationCompute ()
{
	if (m_pDrawSystem)
	{
		m_pDrawSystem->WaitForGpu();
	}

	LOG_MESSAGE("ApplicationCompute  dtor %p", this);
}

void ApplicationCompute ::Update()
{
	BaseType::Update();
	if (m_pDrawSystem)
	{
		auto pFrame = m_pDrawSystem->CreateNewFrame();
		{
			pFrame->ResourceBarrier(m_pComputeOutputTexture.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			auto pShader = m_pComputeShader.get();
			if (pShader)
			{
				auto& buffer = pShader->GetConstant<ACConstantBuffer>(0);// = m_constantBuffer;
				buffer = m_constantBuffer;

				pFrame->SetShader(pShader);
			}
			pFrame->Dispatch(8, 8, 1);
		}
		pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
		{
			pFrame->ResourceBarrier(m_pComputeOutputTexture.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			auto pShader = m_pPresentShader.get();
			if (pShader)
			{
				pFrame->SetShader(pShader);
			}
			auto pGeometry = m_pGeometry.get();
			if (pGeometry)
			{
				pFrame->Draw(pGeometry);
			}
		}
	}
}

void ApplicationCompute ::OnWindowSizeChanged(const int width, const int height)
{
	BaseType::OnWindowSizeChanged(width, height);
	if (m_pDrawSystem)
	{
		m_pDrawSystem->OnResize();
	}

	m_constantBuffer.m_MaxThreadIter[0] = static_cast<float>(width);
	m_constantBuffer.m_MaxThreadIter[1] = static_cast<float>(height);

	return;
}

void ApplicationCompute::OnDestroy(const int exitCode)
{
	PostQuitMessage(exitCode);
}