#include "pch.h"
#include "ApplicationWorldGrid.h"
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

namespace
{
}

IApplication* const ApplicationWorldGrid::Factory(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
{
	return new ApplicationWorldGrid(
		hWnd,
		bFullScreen,
		defaultWidth,
		defaultHeight
		);
}

ApplicationWorldGrid::ApplicationWorldGrid(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
	: IApplication(hWnd, bFullScreen, defaultWidth, defaultHeight)
{
	LOG_MESSAGE("ApplicationWorldGrid  ctor %p", this);

	m_pDrawSystem = std::make_unique< DrawSystem>(hWnd);

	auto pCommandList = m_pDrawSystem->CreateCustomCommandList();

	m_pDrawResources = std::make_unique<DrawResources>();


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
		m_pDrawResources->m_pGeometry = m_pDrawSystem->MakeGeometryGeneric(
			pCommandList->GetCommandList(),
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			inputElementDescArray,
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

		ShaderPipelineStateData shaderPipelineStateData(
			inputElementDescArray,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
			renderTargetFormat,
			CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
			);

		m_pDrawResources->m_pPresentShader = m_pDrawSystem->MakeShader(
			pCommandList->GetCommandList(),
			shaderPipelineStateData,
			pVertexShaderData,
			nullptr,
			pPixelShaderData
			);
	}
}

ApplicationWorldGrid ::~ApplicationWorldGrid ()
{
	if (m_pDrawSystem)
	{
		m_pDrawSystem->WaitForGpu();
	}

	LOG_MESSAGE("ApplicationWorldGrid  dtor %p", this);

	m_pDrawResources.reset();
	m_pDrawSystem.reset();
}

void ApplicationWorldGrid ::Update()
{
	BaseType::Update();
	if (m_pDrawSystem)
	{
		auto pFrame = m_pDrawSystem->CreateNewFrame();
		pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
		{
			auto pShader = m_pDrawResources->m_pPresentShader.get();
			if (pShader)
			{
				pFrame->SetShader(pShader);
			}
			auto pGeometry = m_pDrawResources->m_pGeometry.get();
			if (pGeometry)
			{
				pFrame->Draw(pGeometry);
			}
		}
	}
}

void ApplicationWorldGrid ::OnWindowSizeChanged(const int width, const int height)
{
	BaseType::OnWindowSizeChanged(width, height);
	if (m_pDrawSystem)
	{
		m_pDrawSystem->OnResize();
	}

	return;
}

void ApplicationWorldGrid::OnDestroy(const int exitCode)
{
	PostQuitMessage(exitCode);
}