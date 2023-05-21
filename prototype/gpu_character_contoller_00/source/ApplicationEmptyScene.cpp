#include "pch.h"
#include "ApplicationEmptyScene.h"
#include "Log.h"
#include "FileSystem.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/Shader/Shader.h"
#include "DrawSystemD12/RenderTarget/IRenderTarget.h"
#include "DrawSystemD12/Geometry/GeometryGeneric.h"
#include "DrawSystemD12/DrawSystemFrame.h"
#include "DrawSystemD12/CustomCommandList.h"
#include "DAG2/Dag2Collection.h"
#include "DAG2/Dag2NodeVariable.h"
#include "DAG2/Dag2NodeHelper.h"
#include "DAGRender/DagCalculateCameraRayTexture.h"
#include "DAGRender/DagCalculatePresent.h"

constexpr std::string_view s_dagNodeWidth = "Width";
constexpr std::string_view s_dagNodeHeight = "Height";
constexpr std::string_view s_dagNodeFovWidthRadian = "FovWidthRadian";
constexpr std::string_view s_dagNodeCameraRenderTarget = "CameraRenderTarget";
constexpr std::string_view s_dagNodePersent = "Persent";

IApplication* const ApplicationEmptyScene::Factory(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
{
   return new ApplicationEmptyScene(
		hWnd,
		bFullScreen,
		defaultWidth,
		defaultHeight
	   );
}

ApplicationEmptyScene::ApplicationEmptyScene(
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
   : IApplication(hWnd, bFullScreen, defaultWidth, defaultHeight)
{
	LOG_MESSAGE("ApplicationEmptyScene  ctor %p", this);

	m_pDrawSystem = std::make_unique<DrawSystem>(hWnd);
	const int width = m_pDrawSystem->GetRenderTargetBackBuffer()->GetWidth();
	const int height = m_pDrawSystem->GetRenderTargetBackBuffer()->GetHeight();


	m_pDagCollection = std::make_unique<Dag2Collection>();
	m_pDagWidth = Dag2NodeVariable::Factory<int>(width, Dag2::ValueChanged);
	m_pDagCollection->AddNode(std::string(s_dagNodeWidth), m_pDagWidth);
	m_pDagHeight = Dag2NodeVariable::Factory<int>(height, Dag2::ValueChanged);
	m_pDagCollection->AddNode(std::string(s_dagNodeHeight), m_pDagHeight);
	m_pDagFovWidthRadian = Dag2NodeVariable::Factory<float>(120.0f * (PI / 180.0f), Dag2::ValueChanged);
	m_pDagCollection->AddNode(std::string(s_dagNodeFovWidthRadian), m_pDagFovWidthRadian);

	m_pDagCameraRenderTarget = DagCalculateCameraRayTexture::Factory(*m_pDrawSystem);
	Dag2Collection::SetLinkIndex(m_pDagWidth, 0, m_pDagCameraRenderTarget);
	Dag2Collection::SetLinkIndex(m_pDagHeight, 1, m_pDagCameraRenderTarget);
	Dag2Collection::SetLinkIndex(m_pDagFovWidthRadian, 2, m_pDagCameraRenderTarget);
	m_pDagCollection->AddNode(std::string(s_dagNodeCameraRenderTarget), m_pDagCameraRenderTarget);

	m_pDagPresent = DagCalculatePresent::Factory(*m_pDrawSystem);
	Dag2Collection::SetLinkIndex(m_pDagCameraRenderTarget, 0, m_pDagPresent);
	m_pDagCollection->AddNode(std::string(s_dagNodePersent), m_pDagPresent);
}

ApplicationEmptyScene ::~ApplicationEmptyScene ()
{
	if (m_pDrawSystem)
	{
		m_pDrawSystem->WaitForGpu();
	}

	//m_pDagWidth->Unlink();
	m_pDagWidth.reset();
	//m_pDagHeight->Unlink();
	m_pDagHeight.reset();
	//m_pDagFovWidthRadian->Unlink();
	m_pDagFovWidthRadian.reset();  
	//m_pDagCameraRenderTarget->Unlink();
	m_pDagCameraRenderTarget.reset();
	//m_pDagPresent->Unlink();
	m_pDagPresent.reset();
	m_pDagCollection.reset();

	m_pDrawSystem.reset();

	LOG_MESSAGE("ApplicationEmptyScene  dtor %p", this);
}

void ApplicationEmptyScene ::Update()
{
	BaseType::Update();
	if (nullptr != m_pDagPresent)
	{
		m_pDagPresent->MarkDirty();
		m_pDagPresent->GetValue();
	}
}

void ApplicationEmptyScene ::OnWindowSizeChanged(const int width, const int height)
{
	BaseType::OnWindowSizeChanged(width, height);
	if (m_pDrawSystem)
	{
		m_pDrawSystem->OnResize();
	}

	if (m_pDagWidth)
	{
		Dag2NodeHelper::SetNodeValue<int>(m_pDagWidth, width);
	}
	if (m_pDagHeight)
	{
		Dag2NodeHelper::SetNodeValue<int>(m_pDagHeight, height);
	}

	return;
}

void ApplicationEmptyScene::OnDestroy(const int exitCode)
{
	PostQuitMessage(exitCode);
}