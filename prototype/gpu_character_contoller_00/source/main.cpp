#include "pch.h"
#include "main.h"
#include "WindowHelper.h"
#include "IApplication.h"
#include "ApplicationDAG.h"
#include "ApplicationTriangle.h"
#include "ApplicationTexture.h"
#include "ApplicationCompute.h"



// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	IApplication* pApplication = nullptr;

	TApplicationFactory applicationFactory = [&](
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		)
	{
		//pApplication = ApplicationDAG::Factory(hWnd, bFullScreen, defaultWidth, defaultHeight);
		//pApplication = ApplicationTriangle::Factory(hWnd, bFullScreen, defaultWidth, defaultHeight);
		//pApplication = ApplicationTexture::Factory(hWnd, bFullScreen, defaultWidth, defaultHeight);
		pApplication = ApplicationCompute::Factory(hWnd, bFullScreen, defaultWidth, defaultHeight);
		return pApplication;
	};

	WindowHelper(
		applicationFactory,
		hInstance,
		"GpuCharacterController00",
		false,
		200,
		200,
		nCmdShow
		);


	MSG msg = {};
	int exitCode = 0;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
			{
				exitCode = (int)msg.wParam;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (nullptr != pApplication)
		{
			pApplication->Update();
		}
	}

	return exitCode;
}
