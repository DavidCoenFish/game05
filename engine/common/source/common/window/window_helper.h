#pragma once

class IWindowApplication;
class WindowApplicationParam;

//The return raw pointer IApplication is because ownership is placed under operating system SetWindowLongPtr(hWnd, GWLP_USERDATA,..)
//typedef std::function< std::shared_ptr< IApplication > (const HWND hWnd, const IApplicationParam&) > TApplicationFactory;
typedef std::function< IWindowApplication* (const HWND in_wnd, const WindowApplicationParam&) > TWindowApplicationFactory;

const int WindowHelper(
	const TWindowApplicationFactory& in_application_factory,
	const WindowApplicationParam& in_application_param,
	HINSTANCE in_instance,
	const std::string& in_application_name,
	const int in_cmd_show
	);

