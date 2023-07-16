#include "common/common_pch.h"
#include "common/window/i_window_application.h"
#include "common/window/window_application_param.h"
#include "common/log/log.h"
#include "common/task/task_window.h"

IWindowApplication::IWindowApplication(const HWND in_hwnd, const WindowApplicationParam& in_applicationParam)
   : _hwnd(in_hwnd)
   , _sizemove(false)
   , _suspend(false)
   , _minimized(false)
   , _full_screen(in_applicationParam._full_screen)
   , _default_width(in_applicationParam._width)
   , _default_height(in_applicationParam._height)
   , _task_window(in_applicationParam._task_window)
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

void IWindowApplication::Destroy(const int in_exitCode)
{
	if (nullptr != _task_window)
	{
		_task_window->DestroyApplication(this, in_exitCode);
	}
	return;
}

