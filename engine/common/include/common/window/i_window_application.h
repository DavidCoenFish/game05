#pragma once

class WindowApplicationParam;
class TaskWindow;

class IWindowApplication
{
public:
	IWindowApplication(const HWND in_hwnd, const WindowApplicationParam& in_application_param);
	virtual ~IWindowApplication();

	virtual void Update();
	virtual void OnWindowMoved();
	virtual void OnWindowSizeChanged(const int in_width, const int in_height);
	virtual void OnActivated();
	virtual void OnDeactivated();
	virtual void OnSuspending();
	virtual void OnResuming();
	virtual void OnKey(const int in_vk_code, const int in_scan_code, const bool in_repeat_flag, const int in_repeat_count, bool in_up_flag);

	const int GetDefaultWidth() const { return _default_width; }
	const int GetDefaultHeight() const { return _default_height; }

	const bool GetInSizemove() const { return _sizemove; }
	void SetInSizemove(const bool in_sizemove) { _sizemove = in_sizemove; }

	const bool GetMinimized() const { return _minimized; }
	void SetMinimized(const bool in_minimized) { _minimized = in_minimized; }

	const bool GetFullScreen() const { return _full_screen; }
	void SetFullScreen(const bool in_full_screen) { _full_screen = in_full_screen; }

	void Destroy(const int in_exit_code);

private:
	HWND _hwnd; 
	int _default_width;
	int _default_height;

	bool _sizemove;
	bool _suspend;
	bool _minimized;
	bool _full_screen;

	TaskWindow* const _task_window;

};