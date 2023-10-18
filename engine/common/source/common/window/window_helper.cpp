#include "common/common_pch.h"
#include "common/window/window_helper.h"
#include "common/window/window_application_param.h"
#include "common/window/i_window_application.h"
#include "common/util/utf8.h"
#include "common/log/log.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef std::function< IWindowApplication*(const HWND in_hwnd)> TCreateFunction;

const int WindowHelper(
    const TWindowApplicationFactory& in_application_factory,
    const WindowApplicationParam& in_application_param,
    HINSTANCE in_instance,
    const std::string& in_application_name,
    const int in_cmd_show
)
{
    const std::wstring className(Utf8::Utf8ToUtf16(in_application_name + std::string("Class")));
    const std::wstring name(Utf8::Utf8ToUtf16(in_application_name));

    // Register class
    {
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = in_instance;
        wcex.hIcon = LoadIconW(in_instance, L"IDI_ICON");
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszClassName = className.c_str();
        wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassExW(&wcex))
        {
            return -1;
        }
    }

    // Create window
    {
        RECT rc = { 0, 0, static_cast<LONG>(in_application_param._width), static_cast<LONG>(in_application_param._height) };

        HWND hwnd = 0;
        TCreateFunction createFunction = [=](const HWND in_wnd)
        {
            return in_application_factory(in_wnd, in_application_param);
        };
        DWORD dwStyle = in_application_param._full_screen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
        AdjustWindowRect(&rc, dwStyle, FALSE);
        hwnd = CreateWindowExW(in_application_param._full_screen ? WS_EX_TOPMOST : 0, className.c_str(), name.c_str(), dwStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, 
            nullptr, in_instance, &createFunction);

        if (!hwnd)
        {
            return -1;
        }

        ShowWindow(hwnd, in_application_param._full_screen ? SW_SHOWMAXIMIZED : in_cmd_show);
    }

    return 0;
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND in_hwnd, UINT in_message, WPARAM in_wparam, LPARAM in_lparam)
{
    auto application = reinterpret_cast<IWindowApplication*>(GetWindowLongPtr(in_hwnd, GWLP_USERDATA));
    //LOG_MESSAGE_DEBUG("WndProc message:%d in_wparam:%p in_lparam:%p application:%p", message, in_wparam, in_lparam, application);

    switch (in_message)
    {
    case WM_CREATE:
        {
            auto data = (LPCREATESTRUCTA)(in_lparam);
            TCreateFunction* create_function = data ? (TCreateFunction*)(data->lpCreateParams) : nullptr;
            if (create_function)
            {
                auto application_new = (*create_function)(in_hwnd);
                SetWindowLongPtr(in_hwnd, GWLP_USERDATA, (LONG_PTR)application_new);
            }
        }
        break;

    case WM_PAINT:
        if ((nullptr != application) && (true == application->GetInSizemove()))
        {
            application->Update();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(in_hwnd, &ps);
            EndPaint(in_hwnd, &ps);
        }
        // An application returns zero if it processes this message.
        return 0;

    case WM_MOVE:
        if (nullptr != application)
        {
            application->OnWindowMoved();
        }
        break;

    case WM_SIZE:
        if(nullptr != application)
        {
            if (in_wparam == SIZE_MINIMIZED)
            {
                if (false == application->GetMinimized())
                {
                    application->SetMinimized(true);
                    application->OnSuspending();
                }
            }
            else if (true == application->GetMinimized())
            {
                application->SetMinimized(false);
                application->OnResuming();
            }
            else if (false == application->GetInSizemove())
            {
                application->OnWindowSizeChanged(LOWORD(in_lparam), HIWORD(in_lparam));
            }
        }
        break;

    case WM_ENTERSIZEMOVE:
        if(nullptr != application)
        {
            application->SetInSizemove(true);
        }
        break;

    case WM_EXITSIZEMOVE:
        if(nullptr != application)
        {
            application->SetInSizemove(false);
            RECT rc;
            GetClientRect(in_hwnd, &rc);
            application->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_GETMINMAXINFO:
        if (in_lparam)
        {
            auto info = reinterpret_cast<MINMAXINFO*>(in_lparam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    // equivalent to WM_ACTIVATE
    case WM_ACTIVATEAPP:
        //LOG_MESSAGE_DEBUG("WndProc WM_ACTIVATEAPP:%d", in_wparam);
        if (nullptr != application)
        {
            if (in_wparam)
            {
                application->OnActivated();
            }
            else
            {
                application->OnDeactivated();
            }
        }
        break;

    case WM_POWERBROADCAST:
        switch (in_wparam)
        {
        default:
            break;
        case PBT_APMQUERYSUSPEND:
            if (nullptr != application)
            {
            application->OnSuspending();
            }
            return TRUE;
        case PBT_APMRESUMESUSPEND:
            if ((nullptr != application) && (false == application->GetMinimized()))
            {
                application->OnResuming();
            }
            return TRUE;
        }
        break;

    case WM_MOUSEWHEEL:
        if (nullptr != application)
        {
            const int key_state = GET_KEYSTATE_WPARAM(in_wparam);
            const int z_delta = GET_WHEEL_DELTA_WPARAM(in_wparam);
            application->OnScroll(key_state, z_delta);
        }
        break;

    case WM_DESTROY:
        {
            //WM_QUIT is never sent to window, but you can pull it out of the GetMessage/PeekMessage
            //PostQuitMessage(0);
            if (nullptr != application)
            {
                application->Destroy(0);
            }
            SetWindowLongPtr(in_hwnd, GWLP_USERDATA, (LONG)0);
        }
        break;

    //https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        if(nullptr != application)
        {
            if ((in_wparam == VK_RETURN && (in_lparam & 0x60000000) == 0x20000000))
            {
                // Implements the classic ALT+ENTER fullscreen toggle
                if (application->GetFullScreen())
                {
                    application->SetFullScreen(false);
                    SetWindowLongPtr(in_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                    SetWindowLongPtr(in_hwnd, GWL_EXSTYLE, 0);

                    const int width = application ? application->GetDefaultWidth() : 800;
                    const int height = application ? application->GetDefaultHeight() : 600;

                    ShowWindow(in_hwnd, SW_SHOWNORMAL);
                    SetWindowPos(in_hwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
                else
                {
                    application->SetFullScreen(true);
                    SetWindowLongPtr(in_hwnd, GWL_STYLE, WS_POPUP);
                    SetWindowLongPtr(in_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                    SetWindowPos(in_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                    ShowWindow(in_hwnd, SW_SHOWMAXIMIZED);
                }
            }
            else
            {
                WORD vk_code = LOWORD(in_wparam);                                 // virtual-key code
                WORD key_flags = HIWORD(in_lparam);

                WORD scan_code = LOBYTE(key_flags);                             // scan code
                BOOL is_extended_key = (key_flags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix

                if (is_extended_key)
                {
                    scan_code = MAKEWORD(scan_code, 0xE0);
                }

                BOOL repeat_flag = (key_flags & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
                WORD repeat_count = LOWORD(in_lparam);                            // repeat count, > 0 if several keydown messages was combined into one message

                BOOL up_flag = (key_flags & KF_UP) == KF_UP;                    // transition-state flag, 1 on keyup

                // if we want to distinguish these keys:
                switch (vk_code)
                {
                default:
                    break;
                case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
                case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
                case VK_MENU:    // converts to VK_LMENU or VK_RMENU
                    vk_code = LOWORD(MapVirtualKeyW(scan_code, MAPVK_VSC_TO_VK_EX));
                    break;
                }

                application->OnKey(vk_code, scan_code, repeat_flag, repeat_count, up_flag);
            }
        }
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }

    return DefWindowProc(in_hwnd, in_message, in_wparam, in_lparam);
}
