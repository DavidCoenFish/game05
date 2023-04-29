#pragma once

class ApplicationHolder;
class IApplication;
class IApplicationParam;
class CommandLine;

//The return raw pointer IApplication is because ownership is placed under operating system SetWindowLongPtr(hWnd, GWLP_USERDATA,..)
//typedef std::function< std::shared_ptr< IApplication > (const HWND hWnd, const IApplicationParam&) > TApplicationFactory;
typedef std::function< IApplication* (
	const HWND hWnd, 
	const bool bFullScreen,
	const int defaultWidth,
	const int defaultHeight
	) > TApplicationFactory;

const int WindowHelper(
   const TApplicationFactory& pApplicationFactory,
   HINSTANCE hInstance,
   const std::string& applicationName,
   const bool bFullScreen,
   const int defaultWidth,
   const int defaultHeight,
   const int nCmdShow
   );

