#include "pch.h"
#include "ApplicationCompute.h"
#include "Log.h"
#include "FileSystem.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/Shader/Shader.h"
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
	m_pDrawSystem = std::make_unique< DrawSystem>(hWnd);

	auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
	{
		auto pComputeShaderData = FileSystem::SyncReadFile("ComputeShader.cso");
		ShaderPipelineStateData computePipelineStateData = ShaderPipelineStateData::FactoryComputeShader();

		std::vector< std::shared_ptr< ShaderResourceInfo > > arrayShaderResourceInfo;
		std::vector< std::shared_ptr< ConstantBufferInfo > > arrayShaderConstantsInfo;
		std::vector< std::shared_ptr< UnorderedAccessInfo > > arrayUnorderedAccessInfo;

		m_pShader = m_pDrawSystem->MakeShader(
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
		pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
		auto pShader = m_pShader.get();
		if (pShader)
		{
			pFrame->SetShader(pShader);
		}
		pFrame->Dispatch(8, 8, 1);
	}
}

void ApplicationCompute ::OnWindowSizeChanged(const int width, const int height)
{
	BaseType::OnWindowSizeChanged(width, height);
	if (m_pDrawSystem)
	{
		m_pDrawSystem->OnResize();
	}

	return;
}

void ApplicationCompute::OnDestroy(const int exitCode)
{
	PostQuitMessage(exitCode);
}