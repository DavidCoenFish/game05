#include "pch.h"
#include "main.h"
#include "WindowHelper.h"
#include "IApplication.h"
#include "ApplicationTriangle.h"



// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WindowHelper(
		ApplicationTriangle::Factory,
		hInstance,
		"GpuCharacterController00",
		false,
		200,
		200,
		nCmdShow
		);


	const int result = 0; //RunTask(hInstance, nCmdShow);

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
   }

	return result;
}
