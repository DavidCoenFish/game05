#include "pch.h"
#include "ApplicationTriangle.h"
#include "Log.h"

IApplication* const ApplicationTriangle::Factory(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
{
   return new ApplicationTriangle(
		hWnd,
		bFullScreen,
		defaultWidth,
		defaultHeight
	   );
}

ApplicationTriangle::ApplicationTriangle(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
   : IApplication(hWnd, bFullScreen, defaultWidth, defaultHeight)
{
   LOG_MESSAGE("ApplicationTriangle  ctor %p", this);
   //m_pDrawSystem = std::make_unique< DrawSystem>(hWnd);

   //std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray;
   //inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
   //   "POSITION",
   //   0,
   //   DXGI_FORMAT_R32G32_FLOAT,
   //   0,
   //   D3D12_APPEND_ALIGNED_ELEMENT,
   //   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
   //   0 // UINT InstanceDataStepRate;
   //   });
   //inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
   //   "COLOR",
   //   0,
   //   DXGI_FORMAT_R32G32B32A32_FLOAT,
   //   0,
   //   D3D12_APPEND_ALIGNED_ELEMENT,
   //   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
   //   0 // UINT InstanceDataStepRate;
   //   });

   //auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
   //{
   //   auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "VertexShader.cso");
   //   auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "PixelShader.cso");
   //   std::vector<DXGI_FORMAT> renderTargetFormat;
   //   renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

   //   ShaderPipelineStateData shaderPipelineStateData(
   //      inputElementDescArray,
   //      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
   //      DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
   //      renderTargetFormat,
   //      CD3DX12_BLEND_DESC(D3D12_DEFAULT),
   //      CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
   //      CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
   //      );

   //   m_pShader = m_pDrawSystem->MakeShader(
   //      pCommandList->GetCommandList(),
   //      shaderPipelineStateData,
   //      pVertexShaderData,
   //      nullptr,
   //      pPixelShaderData
   //      );
   //}

   //{
   //   std::vector< Vertex > vertexData({
   //      {{0.0f,0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}}, 
   //      {{0.5f,-0.5}, {0.0f, 1.0f, 0.0f, 1.0f}},
   //      {{-0.5f,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}},
   //      });
   //   m_pGeometry = m_pDrawSystem->MakeGeometry(
   //      pCommandList->GetCommandList(),
   //      D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
   //      inputElementDescArray,
   //      vertexData
   //      );
   //}
}

ApplicationTriangle ::~ApplicationTriangle ()
{
   //if (m_pDrawSystem)
   //{
   //   m_pDrawSystem->WaitForGpu();
   //}
   //m_pShader.reset();
   //m_pGeometry.reset();
   //m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationTriangle  dtor %p", this);
}

void ApplicationTriangle ::Update()
{
   BaseType::Update();
   //if (m_pDrawSystem)
   //{
   //   auto pFrame = m_pDrawSystem->CreateNewFrame();
   //   pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
   //   pFrame->SetShader(m_pShader.get());
   //   pFrame->Draw(m_pGeometry.get());
   //}
}

void ApplicationTriangle ::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   //if (m_pDrawSystem)
   //{
   //   m_pDrawSystem->OnResize();
   //}

   return;
}

void ApplicationTriangle::OnDestroy(const int exitCode)
{
	PostQuitMessage(exitCode);
}