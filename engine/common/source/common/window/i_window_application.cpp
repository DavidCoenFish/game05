#include "common/common_pch.h"
#include "common/window/i_window_application.h"
#include "common/window/window_application_param.h"
#include "common/log/log.h"
#include "common/task/task_window.h"

IWindowApplication::IWindowApplication(const HWND hWnd, const WindowApplicationParam& applicationParam)
   : _wnd(hWnd)
   , _sizemove(false)
   , _suspend(false)
   , _minimized(false)
   , _full_screen(applicationParam._full_screen)
   , _default_width(applicationParam._width)
   , _default_height(applicationParam._height)
   , _task_window(applicationParam._task_window)
{
   LOG_MESSAGE("IWindowApplication ctor %p", this);
}

IWindowApplication::~IWindowApplication()
{
   LOG_MESSAGE("IWindowApplication dtor %p", this);
}

void IWindowApplication::Update()
{
   return;
}

void IWindowApplication::OnWindowMoved()
{
   return;
}

void IWindowApplication::OnWindowSizeChanged(const int, const int)
{
   return;
}

void IWindowApplication::OnActivated()
{
   return;
}

void IWindowApplication::OnDeactivated()
{
   return;
}

void IWindowApplication::OnSuspending()
{
   return;
}

void IWindowApplication::OnResuming()
{
   return;
}

void IWindowApplication::OnKey(const int, const int, const bool, const int, bool)
{
   return;
}

void IWindowApplication::Destroy(const int exitCode)
{
	if (nullptr != _task_window)
	{
		_task_window->DestroyApplication(this, exitCode);
	}
	return;
}

