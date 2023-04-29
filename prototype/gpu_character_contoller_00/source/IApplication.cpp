#include "pch.h"
#include "IApplication.h"
#include "Log.h"

IApplication::IApplication(
	const HWND hWnd, 
	const bool startFullScreen,
	const int defaultWidth,
	const int defaultHeight
	)
   : m_hWnd(hWnd)
   , m_bFullScreen(startFullScreen)
   , m_defaultWidth(defaultWidth)
   , m_defaultHeight(defaultHeight)
   , m_bInSizemove(false)
   , m_bInSuspend(false)
   , m_bMinimized(false)
{
   LOG_MESSAGE("IApplication ctor %p", this);
}

IApplication::~IApplication()
{
   LOG_MESSAGE("IApplication dtor %p", this);
}

void IApplication::Update()
{
   return;
}

void IApplication::OnWindowMoved()
{
   return;
}

void IApplication::OnWindowSizeChanged(const int, const int)
{
   return;
}

void IApplication::OnActivated()
{
   return;
}

void IApplication::OnDeactivated()
{
   return;
}

void IApplication::OnSuspending()
{
   return;
}

void IApplication::OnResuming()
{
   return;
}

void IApplication::OnKey(const int, const int, const bool, const int, bool)
{
   return;
}

void IApplication::OnDestroy(const int)
{
	return;
}

