#include "pch.h"
#include "ApplicationTexture.h"
#include "Log.h"
#include "FileSystem.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/Shader/Shader.h"
#include "DrawSystemD12/Shader/ShaderResource.h"
#include "DrawSystemD12/Shader/ShaderResourceInfo.h"
#include "DrawSystemD12/Geometry/GeometryGeneric.h"
#include "DrawSystemD12/DrawSystemFrame.h"
#include "DrawSystemD12/CustomCommandList.h"

IApplication* const ApplicationTexture::Factory(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
{
	return new ApplicationTexture(
		hWnd,
		bFullScreen,
		defaultWidth,
		defaultHeight
		);
}

ApplicationTexture::ApplicationTexture(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
	: IApplication(hWnd, bFullScreen, defaultWidth, defaultHeight)
{
	LOG_MESSAGE("ApplicationTexture  ctor %p", this);
	m_pDrawSystem = std::make_unique< DrawSystem>(
		hWnd,
		2, //const unsigned int backBufferCount,
		D3D_FEATURE_LEVEL_11_0, //const D3D_FEATURE_LEVEL d3dFeatureLevel,
		0, //const unsigned int options
		RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM, true, VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f))//const RenderTargetFormatData& targetFormatData = RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM),
		);

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

	auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
	{
		auto pShaderHeapWrapper = m_pDrawSystem->MakeHeapWrapperCbvSrvUav(m_pDrawSystem->GetBackBufferCount());
		D3D12_RESOURCE_DESC desc = {
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
			D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
			256, //UINT64 Width;
			256, //UINT Height;
			1, //UINT16 DepthOrArraySize;
			1, //UINT16 MipLevels;
			DXGI_FORMAT_B8G8R8A8_UNORM, //DXGI_FORMAT Format;
			DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
			D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
			D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAGS Flags;
		};
		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		std::vector<uint8_t> data;
		for (int y = 0; y < 256; ++y)
		{
		for (int x = 0; x < 256; ++x)
		{
			uint8_t red = (uint8_t)x;
			uint8_t green = (uint8_t)y;
			uint8_t blue = (uint8_t)((0 != (y & 0x10)) == (0 != (x & 0x10)) ? 255 : 0);
			uint8_t alpha = (uint8_t)255;
			data.push_back(red);
			data.push_back(green);
			data.push_back(blue);
			data.push_back(alpha);
		}
		}

		m_pTexture = m_pDrawSystem->MakeShaderResource(
		pCommandList->GetCommandList(),
		pShaderHeapWrapper,
		desc,
		srvDesc,
		data
		);
	}


	{
		m_pGeometry = m_pDrawSystem->MakeGeometryGeneric(
			pCommandList->GetCommandList(),
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			inputElementDescArray,
			std::vector<float>({
				0.0f,0.5f, 1.0f, 0.5f, 
				0.5f,-0.5f, 0.0f, 1.0f,
				-0.5f,-0.5f, 0.0f, 0.0f
				}),
			4);
	}

	
	{
		auto pVertexShaderData = FileSystem::SyncReadFile("TextureVertexShader.cso");
		auto pPixelShaderData = FileSystem::SyncReadFile("TexturePixelShader.cso");
		std::vector<DXGI_FORMAT> renderTargetFormat;
		renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

		std::vector< std::shared_ptr< ShaderResourceInfo > > arrayTexture;
		arrayTexture.push_back( ShaderResourceInfo::FactorySampler( m_pTexture->GetHeapWrapperItem(), D3D12_SHADER_VISIBILITY_PIXEL ) );

		ShaderPipelineStateData shaderPipelineStateData(
			inputElementDescArray,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
			renderTargetFormat,
			CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
			);

		m_pShader = m_pDrawSystem->MakeShader(
			pCommandList->GetCommandList(),
			shaderPipelineStateData,
			pVertexShaderData,
			nullptr,
			pPixelShaderData,
			arrayTexture
			);
	}

}

ApplicationTexture ::~ApplicationTexture ()
{
	if (m_pDrawSystem)
	{
		m_pDrawSystem->WaitForGpu();
	}
	m_pShader.reset();
	m_pTexture.reset();
	m_pGeometry.reset();
	m_pDrawSystem.reset();

	LOG_MESSAGE("ApplicationTexture  dtor %p", this);
}

void ApplicationTexture ::Update()
{
	BaseType::Update();
	if (m_pDrawSystem)
	{
		auto pFrame = m_pDrawSystem->CreateNewFrame();
		pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
		auto pShader = m_pShader.get();
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

void ApplicationTexture ::OnWindowSizeChanged(const int width, const int height)
{
	BaseType::OnWindowSizeChanged(width, height);
	if (m_pDrawSystem)
	{
		m_pDrawSystem->OnResize();
	}

	return;
}

void ApplicationTexture::OnDestroy(const int exitCode)
{
	PostQuitMessage(exitCode);
}