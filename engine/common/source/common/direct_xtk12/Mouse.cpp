// --------------------------------------------------------------------------------------
// File: Mouse.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkId=248929
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#include "common/common_pch.h"

#include "common/direct_xtk12/mouse.h"
#include "common/direct_xtk12/handle_closer.h"
using namespace DirectX;
using Microsoft::WRL::ComPtr;
#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
    #include <GameInput.h>

    // ======================================================================================
    // Win32 + GameInput implementation
    // ======================================================================================
    //
    // Call this static function from your Window Message Procedure
    //
    // LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    // {
    // Switch (message)
    // {
    // Case WM_ACTIVATEAPP:
    // Case WM_MOUSEMOVE:
    // Case WM_LBUTTONDOWN:
    // Case WM_LBUTTONUP:
    // Case WM_RBUTTONDOWN:
    // Case WM_RBUTTONUP:
    // Case WM_MBUTTONDOWN:
    // Case WM_MBUTTONUP:
    // Case WM_MOUSEWHEEL:
    // Case WM_XBUTTONDOWN:
    // Case WM_XBUTTONUP:
    // Mouse::ProcessMessage(message, wParam, lParam);
    // Break;
    //
    // }
    // }
    //
    class Mouse::Impl
    {
    public:
        explicit Impl(Mouse* in_owner) noexcept (false) 
            : _state{}
            , _owner(in_owner)
            , _is4k(false)
            , _connected(0)
            , _device_token(0)
            , _mode(MODE_ABSOLUTE)
            , _scroll_wheel_current(0)
            , _relative_x(INT64_MAX)
            , _relative_y(INT64_MAX)
            , _relative_wheel_y(INT64_MAX)
        {
            if (s_mouse)
            {
                throw std::exception("Mouse is a singleton");
            }
            s_mouse = this;
            ThrowIfFailed(GameInputCreate(_game_input.GetAddressOf()));
            ThrowIfFailed(_game_input->RegisterDeviceCallback(
                nullptr,
                GameInputKindMouse,
                GameInputDeviceConnected,
                GameInputBlockingEnumeration,
                this,
                OnGameInputDevice,
                &_device_token
                ));
            _scroll_wheel_value.reset(CreateEventEx(
                nullptr,
                nullptr,
                CREATE_EVENT_MANUAL_RESET,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            if (!_scroll_wheel_value)
            {
                throw std::exception("CreateEventEx");
            }
        }

        Impl(Impl &&) = default;
        Impl& operator=(Impl &&) = default;
        Impl(Impl const&) = delete;
        Impl& operator=(Impl const&) = delete;
        ~Impl()
        {
            if (_device_token)
            {
                if (_game_input)
                {
                    HRESULT hr = _game_input->UnregisterCallback(
                        _device_token,
                        UINT64_MAX
                        );
                    if (FAILED(hr))
                    {
                        DebugTrace(
                            "ERROR: GameInput::UnregisterCallback [mouse] failed (%08X)",
                            static_cast < unsigned int > (hr)
                            );
                    }
                }
                _device_token = 0;
            }
            s_mouse = nullptr;
        }

        void GetState(State&in_state) const
        {
            memcpy(
                &in_state,
                &_state,
                sizeof (State)
                );
            in_state._position_mode = _mode;
            DWORD result = WaitForSingleObjectEx(
                _scroll_wheel_value.get(),
                0,
                FALSE
                );
            if (result == WAIT_FAILED) throw std::exception("WaitForSingleObjectEx");
            if (result == WAIT_OBJECT_0)
            {
                _scroll_wheel_current = 0;
            }
            if (in_state._position_mode == MODE_RELATIVE)
            {
                in_state._x = in_state._y = 0;
                ComPtr < IGameInputReading > reading;
                if (SUCCEEDED(_game_input->GetCurrentReading(
                    GameInputKindMouse,
                    nullptr,
                    reading.GetAddressOf()
                    )))
                {
                    GameInputMouseState mouse;
                    if (reading->GetMouseState(&mouse))
                    {
                        in_state._left_button = (mouse._buttons&GameInputMouseLeftButton) != 0;
                        in_state._middle_button = (mouse._buttons&GameInputMouseMiddleButton) != 0;
                        in_state._right_button = (mouse._buttons&GameInputMouseRightButton) != 0;
                        in_state._x_button1 = (mouse._buttons&GameInputMouseButton4) != 0;
                        in_state._x_button2 = (mouse._buttons&GameInputMouseButton5) != 0;
                        if (_relative_x != INT64_MAX)
                        {
                            in_state._x = static_cast < int > (mouse._position_x - _relative_x);
                            in_state._y = static_cast < int > (mouse._position_y - _relative_y);
                            int scroll_delta = static_cast < int > (mouse._wheel_y - _relative_wheel_y);
                            _scroll_wheel_current += scroll_delta;
                        }
                        _relative_x = mouse._position_x;
                        _relative_y = mouse._position_y;
                        _relative_wheel_y = mouse._wheel_y;
                    }
                }
            }
            in_state._scroll_wheel_value = _scroll_wheel_current;
        }

        void ResetScrollWheelValue() noexcept
        {
            SetEvent(_scroll_wheel_value.get());
        }

        void SetMode(Mode in_mode)
        {
            if (_mode == in_mode) return;
            _mode = in_mode;
            _relative_x = INT64_MAX;
            _relative_y = INT64_MAX;
            _relative_wheel_y = INT64_MAX;
            ShowCursor((in_mode == MODE_ABSOLUTE) ? TRUE : FALSE);
        }

        bool IsConnected() const noexcept
        {
            return _connected > 0;
        }

        bool IsVisible() const noexcept
        {
            if (_mode == MODE_RELATIVE) return false;
            CURSORINFO info =
            {
                sizeof (CURSORINFO), 0, nullptr, {}
            };
            if (!GetCursorInfo(&info)) 
                return false;
            return (info._flags&CURSOR_SHOWING) != 0;
        }

        void SetVisible(bool in_visible)
        {
            if (_mode == MODE_RELATIVE) return;
            CURSORINFO info =
            {
                sizeof (CURSORINFO), 0, nullptr, {}
            };
            if (!GetCursorInfo(&info))
            {
                throw std::exception("GetCursorInfo");
            }
            bool isvisible = (info._flags&CURSOR_SHOWING) != 0;
            if (isvisible != in_visible)
            {
                ShowCursor(in_visible);
            }
        }


    private:
        friend void Mouse::ProcessMessage(
            UINT in_message,
            WPARAM in_w_param,
            LPARAM in_l_param
            );
        static void CALLBACK OnGameInputDevice(
            _In_ GameInputCallbackToken,
            _In_ void* in_context,
            _In_ IGameInputDevice*,
            _In_ uint64_t,
            _In_ GameInputDeviceStatus in_current_status,
            _In_ GameInputDeviceStatus
            ) noexcept
        {
            auto impl = reinterpret_cast < Mouse::Impl* > (in_context);
            if (in_current_status&GameInputDeviceConnected)
            {
                ++impl->_connected;
            }
            else if (impl->_connected > 0)
            {
                --impl->_connected;
            }

        }


    public:
        State _state;
        Mouse* _owner;
        bool _is4k;
        uint32_t _connected;
        static Mouse::Impl* s_mouse;

    private:
        ComPtr < IGameInput > _game_input;
        GameInputCallbackToken _device_token;
        Mode _mode;
        std::unique_ptr < void, HandleCloser > _scroll_wheel_value;
        mutable int _scroll_wheel_current;
        mutable _int64_t _relative_x;
        mutable _int64_t _relative_y;
        mutable _int64_t _relative_wheel_y;
    };
    Mouse::Impl* Mouse::Impl::s_mouse = nullptr;
    void Mouse::ProcessMessage(
        UINT in_message,
        WPARAM in_w_param,
        LPARAM in_l_param
        )
    {
        auto impl = Impl::s_mouse;
        if (!impl) return;
        DWORD result = WaitForSingleObjectEx(
            impl->_scroll_wheel_value.get(),
            0,
            FALSE
            );
        if (result == WAIT_FAILED) throw std::exception("WaitForSingleObjectEx");
        if (result == WAIT_OBJECT_0)
        {
            impl->_scroll_wheel_current = 0;
        }
        switch (in_message)
        {
        case WM_ACTIVATEAPP:
            if (in_w_param)
            {
                if (impl->_mode == MODE_RELATIVE)
                {
                    impl->_relative_x = INT64_MAX;
                    impl->_relative_y = INT64_MAX;
                    ShowCursor(FALSE);
                }
            }
            else
            {
                memset(
                    &impl->_state,
                    0,
                    sizeof (State)
                    );
            }
            return;
        case WM_MOUSEMOVE:
            break;
        case WM_LBUTTONDOWN:
            impl->_state._left_button = true;
            break;
        case WM_LBUTTONUP:
            impl->_state._left_button = false;
            break;
        case WM_RBUTTONDOWN:
            impl->_state._right_button = true;
            break;
        case WM_RBUTTONUP:
            impl->_state._right_button = false;
            break;
        case WM_MBUTTONDOWN:
            impl->_state._middle_button = true;
            break;
        case WM_MBUTTONUP:
            impl->_state._middle_button = false;
            break;
        case WM_MOUSEWHEEL:
            if (impl->_mode == MODE_ABSOLUTE)
            {
                impl->_scroll_wheel_current += GET_WHEEL_DELTA_WPARAM(in_w_param);
            }
            return;
        case WM_XBUTTONDOWN:
            switch (GET_XBUTTON_WPARAM(in_w_param))
            {
            case XBUTTON1:
                impl->_state._x_button1 = true;
                break;
            case XBUTTON2:
                impl->_state._x_button2 = true;
                break;
            }

            break;
        case WM_XBUTTONUP:
            switch (GET_XBUTTON_WPARAM(w_param))
            {
            case XBUTTON1:
                impl->_state._x_button1 = false;
                break;
            case XBUTTON2:
                impl->_state._x_button2 = false;
                break;
            }

            break;
        default:
            // Not a mouse message, so exit
            return;
        }

        if (impl->_mode == MODE_ABSOLUTE)
        {
            // All mouse messages provide a new pointer position
            int x_pos = static_cast < short > (LOWORD(l_param));
            // GET_X_LPARAM(lParam);
            int y_pos = static_cast < short > (HIWORD(l_param));
            // GET_Y_LPARAM(lParam);
            if (impl->_is4k)
            {
                impl->_state._x = static_cast < int > (x_pos) * 2;
                impl->_state._y = static_cast < int > (y_pos) * 2;
            }
            else
            {
                impl->_state._x = static_cast < int > (x_pos);
                impl->_state._y = static_cast < int > (y_pos);
            }
        }
    }

    void Mouse::SetResolution(bool in_use4k)
    {
        auto impl = Impl::s_mouse;
        if (!impl) return;
        impl->_is4k = in_use4k;
    }

#elif !defined (WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
    // ======================================================================================
    // Win32 desktop implementation
    // ======================================================================================
    //
    // For a Win32 desktop application, in your window setup be sure to call this method:
    //
    // M_mouse->SetWindow(hwnd);
    //
    // And call this static function from your Window Message Procedure
    //
    // LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    // {
    // Switch (message)
    // {
    // Case WM_ACTIVATEAPP:
    // Case WM_INPUT:
    // Case WM_MOUSEMOVE:
    // Case WM_LBUTTONDOWN:
    // Case WM_LBUTTONUP:
    // Case WM_RBUTTONDOWN:
    // Case WM_RBUTTONUP:
    // Case WM_MBUTTONDOWN:
    // Case WM_MBUTTONUP:
    // Case WM_MOUSEWHEEL:
    // Case WM_XBUTTONDOWN:
    // Case WM_XBUTTONUP:
    // Case WM_MOUSEHOVER:
    // Mouse::ProcessMessage(message, wParam, lParam);
    // Break;
    //
    // }
    // }
    //
    class Mouse::Impl
    {
    public:
        explicit Impl(Mouse* in_owner) noexcept (false) 
            : _state{}
            , _owner(in_owner)
            , _window(nullptr)
            , _mode(MODE_ABSOLUTE)
            , _last_x(0)
            , _last_y(0)
            , _relative_x(INT32_MAX)
            , _relative_y(INT32_MAX)
            , _in_focus(true)
        {
            if (s_mouse)
            {
                throw std::exception("Mouse is a singleton");
            }
            s_mouse = this;
            _scroll_wheel_value.reset(CreateEventEx(
                nullptr,
                nullptr,
                CREATE_EVENT_MANUAL_RESET,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            _relative_read.reset(CreateEventEx(
                nullptr,
                nullptr,
                CREATE_EVENT_MANUAL_RESET,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            _absolute_mode.reset(CreateEventEx(
                nullptr,
                nullptr,
                0,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            _relative_mode.reset(CreateEventEx(
                nullptr,
                nullptr,
                0,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            if (!_scroll_wheel_value || !_relative_read || !_absolute_mode || !_relative_mode)
            {
                throw std::exception("CreateEventEx");
            }
        }

        Impl(Impl &&) = default;
        Impl& operator=(Impl &&) = default;
        Impl(Impl const&) = delete;
        Impl& operator=(Impl const&) = delete;
        ~Impl()
        {
            s_mouse = nullptr;
        }

        void GetState(State&in_state) const
        {
            memcpy(
                &in_state,
                &_state,
                sizeof (State)
                );
            in_state._position_mode = _mode;
            DWORD result = WaitForSingleObjectEx(
                _scroll_wheel_value.get(),
                0,
                FALSE
                );
            if (result == WAIT_FAILED) throw std::exception("WaitForSingleObjectEx");
            if (result == WAIT_OBJECT_0)
            {
                in_state._scroll_wheel_value = 0;
            }
            if (in_state._position_mode == MODE_RELATIVE)
            {
                result = WaitForSingleObjectEx(
                    _relative_read.get(),
                    0,
                    FALSE
                    );
                if (result == WAIT_FAILED) throw std::exception("WaitForSingleObjectEx");
                if (result == WAIT_OBJECT_0)
                {
                    in_state._x = 0;
                    in_state._y = 0;
                }
                else
                {
                    SetEvent(_relative_read.get());
                }
            }
        }

        void ResetScrollWheelValue() noexcept
        {
            SetEvent(_scroll_wheel_value.get());
        }

        void SetMode(Mode in_mode)
        {
            if (_mode == in_mode) return;
            SetEvent((in_mode == MODE_ABSOLUTE) ? _absolute_mode.get() : _relative_mode.get());
            assert(_window != nullptr);
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof (tme);
            tme.dwFlags = TME_HOVER;
            tme.hwndTrack = _window;
            tme.dwHoverTime = 1;
            if (!TrackMouseEvent(&tme))
            {
                throw std::exception("TrackMouseEvent");
            }
        }

        bool IsConnected() const noexcept
        {
            return GetSystemMetrics(SM_MOUSEPRESENT) != 0;
        }

        bool IsVisible() const noexcept
        {
            if (_mode == MODE_RELATIVE) return false;
            CURSORINFO info =
            {
                sizeof (CURSORINFO), 0, nullptr, {}
            };
            if (!GetCursorInfo(&info)) 
            {
                return false;
            }
            return (info.flags & CURSOR_SHOWING) != 0;
        }

        void SetVisible(bool in_visible)
        {
            if (_mode == MODE_RELATIVE) return;
            CURSORINFO info =
            {
                sizeof (CURSORINFO), 0, nullptr, {}
            };
            if (!GetCursorInfo(&info))
            {
                throw std::exception("GetCursorInfo");
            }
            bool isvisible = (info.flags & CURSOR_SHOWING) != 0;
            if (isvisible != in_visible)
            {
                ShowCursor(in_visible);
            }
        }

        void SetWindow(HWND in_window)
        {
            if (_window == in_window) return;
            assert(in_window != nullptr);
            RAWINPUTDEVICE Rid;
            Rid.usUsagePage = 0x1; /* HID_USAGE_PAGE_GENERIC */
            Rid.usUsage = 0x2; /* HID_USAGE_GENERIC_MOUSE */
            Rid.dwFlags = RIDEV_INPUTSINK;
            Rid.hwndTarget = in_window;
            if (!RegisterRawInputDevices(
                &Rid,
                1,
                sizeof (RAWINPUTDEVICE)
                ))
            {
                throw std::exception("RegisterRawInputDevices");
            }
            _window = in_window;
        }


    private:
        friend void Mouse::ProcessMessage(
            UINT in_message,
            WPARAM in_w_param,
            LPARAM in_l_param
            );
        void ClipToWindow() noexcept
        {
            assert(_window != nullptr);
            RECT rect;
            GetClientRect(
                _window,
                &rect
                );
            POINT ul;
            ul.x = rect.left;
            ul.y = rect.top;
            POINT lr;
            lr.x = rect.right;
            lr.y = rect.bottom;
            MapWindowPoints(
                _window,
                nullptr,
                &ul,
                1
                );
            MapWindowPoints(
                _window,
                nullptr,
                &lr,
                1
                );
            rect.left = ul.x;
            rect.top = ul.y;
            rect.right = lr.x;
            rect.bottom = lr.y;
            ClipCursor(&rect);
        }


    public:
        State _state;
        Mouse* _owner;
        static Mouse::Impl* s_mouse;

    private:
        HWND _window;
        Mode _mode;
        std::unique_ptr < void, HandleCloser > _scroll_wheel_value;
        std::unique_ptr < void, HandleCloser > _relative_read;
        std::unique_ptr < void, HandleCloser > _absolute_mode;
        std::unique_ptr < void, HandleCloser > _relative_mode;
        int _last_x;
        int _last_y;
        int _relative_x;
        int _relative_y;
        bool _in_focus;
    };
    Mouse::Impl* Mouse::Impl::s_mouse = nullptr;
    void Mouse::SetWindow(HWND in_window)
    {
        _impl->SetWindow(in_window);
    }

    void Mouse::ProcessMessage(
        UINT in_message,
        WPARAM in_w_param,
        LPARAM in_l_param
        )
    {
        auto impl = Impl::s_mouse;
        if (!impl) return;
        HANDLE events[3] =
        {
            impl->_scroll_wheel_value.get(), impl->_absolute_mode.get(), impl->_relative_mode.get()};
        switch (WaitForMultipleObjectsEx(
            _countof(events),
            events,
            FALSE,
            0,
            FALSE
            ))
        {
        default:
        case WAIT_TIMEOUT:
            break;
        case WAIT_OBJECT_0:
            impl->_state._scroll_wheel_value = 0;
            ResetEvent(events[0]);
            break;
        case (WAIT_OBJECT_0 + 1):
            {
                impl->_mode = MODE_ABSOLUTE;
                ClipCursor(nullptr);
                POINT point;
                point.x = impl->_last_x;
                point.y = impl->_last_y;
                // We show the cursor before moving it to support Remote Desktop
                ShowCursor(TRUE);
                if (MapWindowPoints(
                    impl->_window,
                    nullptr,
                    &point,
                    1
                    ))
                {
                    SetCursorPos(
                        point.x,
                        point.y
                        );
                }
                impl->_state._x = impl->_last_x;
                impl->_state._y = impl->_last_y;
            }

            break;
            case (WAIT_OBJECT_0 + 2) 
                :
            {
                ResetEvent(impl->_relative_read.get());
                impl->_mode = MODE_RELATIVE;
                impl->_state._x = impl->_state._y = 0;
                impl->_relative_x = INT32_MAX;
                impl->_relative_y = INT32_MAX;
                ShowCursor(FALSE);
                impl->ClipToWindow();
            }

            break;
        case WAIT_FAILED:
            throw std::exception("WaitForMultipleObjectsEx");
        }

        switch (in_message)
        {
        case WM_ACTIVATEAPP:
            if (in_w_param)
            {
                impl->_in_focus = true;
                if (impl->_mode == MODE_RELATIVE)
                {
                    impl->_state._x = impl->_state._y = 0;
                    ShowCursor(FALSE);
                    impl->ClipToWindow();
                }
            }
            else
            {
                int scroll_wheel = impl->_state._scroll_wheel_value;
                memset(
                    &impl->_state,
                    0,
                    sizeof (State)
                    );
                impl->_state._scroll_wheel_value = scroll_wheel;
                impl->_in_focus = false;
            }
            return;
        case WM_INPUT:
            if (impl->_in_focus && impl->_mode == MODE_RELATIVE)
            {
                RAWINPUT raw;
                UINT raw_size = sizeof (raw);
                UINT result_data = GetRawInputData(
                    reinterpret_cast < HRAWINPUT > (in_l_param),
                    RID_INPUT,
                    &raw,
                    &raw_size,
                    sizeof (RAWINPUTHEADER)
                    );
                if (result_data == UINT(- 1))
                {
                    throw std::exception("GetRawInputData");
                }
                if (raw.header.dwType == RIM_TYPEMOUSE)
                {
                    if (! (raw.data.mouse.usFlags&MOUSE_MOVE_ABSOLUTE))
                    {
                        impl->_state._x = raw.data.mouse.lLastX;
                        impl->_state._y = raw.data.mouse.lLastY;
                        ResetEvent(impl->_relative_read.get());
                    }
                    else if (raw.data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)
                    {
                        // This is used to make Remote Desktop sessons work
                        const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                        const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                        int x = static_cast < int > ((float (raw.data.mouse.lLastX) / 65535.0f) * float (width));
                        int y = static_cast < int > ((float (raw.data.mouse.lLastY) / 65535.0f) * float (height));
                        if (impl->_relative_x == INT32_MAX)
                        {
                            impl->_state._x = impl->_state._y = 0;
                        }
                        else
                        {
                            impl->_state._x = x - impl->_relative_x;
                            impl->_state._y = y - impl->_relative_y;
                        }
                        impl->_relative_x = x;
                        impl->_relative_y = y;
                        ResetEvent(impl->_relative_read.get());
                    }

                }
            }
            return;
        case WM_MOUSEMOVE:
            break;
        case WM_LBUTTONDOWN:
            impl->_state._left_button = true;
            break;
        case WM_LBUTTONUP:
            impl->_state._left_button = false;
            break;
        case WM_RBUTTONDOWN:
            impl->_state._right_button = true;
            break;
        case WM_RBUTTONUP:
            impl->_state._right_button = false;
            break;
        case WM_MBUTTONDOWN:
            impl->_state._middle_button = true;
            break;
        case WM_MBUTTONUP:
            impl->_state._middle_button = false;
            break;
        case WM_MOUSEWHEEL:
            impl->_state._scroll_wheel_value += GET_WHEEL_DELTA_WPARAM(in_w_param);
            return;
        case WM_XBUTTONDOWN:
            switch (GET_XBUTTON_WPARAM(in_w_param))
            {
            case XBUTTON1:
                impl->_state._x_button1 = true;
                break;
            case XBUTTON2:
                impl->_state._x_button2 = true;
                break;
            }

            break;
        case WM_XBUTTONUP:
            switch (GET_XBUTTON_WPARAM(in_w_param))
            {
            case XBUTTON1:
                impl->_state._x_button1 = false;
                break;
            case XBUTTON2:
                impl->_state._x_button2 = false;
                break;
            }

            break;
        case WM_MOUSEHOVER:
            break;
        default:
            // Not a mouse message, so exit
            return;
        }

        if (impl->_mode == MODE_ABSOLUTE)
        {
            // All mouse messages provide a new pointer position
            int x_pos = static_cast < short > (LOWORD(in_l_param));
            // GET_X_LPARAM(lParam);
            int y_pos = static_cast < short > (HIWORD(in_l_param));
            // GET_Y_LPARAM(lParam);
            impl->_state._x = impl->_last_x = x_pos;
            impl->_state._y = impl->_last_y = y_pos;
        }
    }

#elif defined (_XBOX_ONE) && (!defined (_TITLE) || (_XDK_VER < 0x42D907D1))
    // ======================================================================================
    // Null device
    // ======================================================================================
    class Mouse::Impl
    {
    public:
        explicit Impl(Mouse* in_owner) noexcept (false) 
            : _owner(in_owner)
        {
            if (s_mouse)
            {
                throw std::exception("Mouse is a singleton");
            }
            s_mouse = this;
        }

        ~Impl()
        {
            s_mouse = nullptr;
        }

        void GetState(State&in_state) const
        {
            memset(
                &in_state,
                0,
                sizeof (State)
                );
        }

        void ResetScrollWheelValue() noexcept{}

        void SetMode(Mode){}

        bool IsConnected() const
        {
            return false;
        }

        bool IsVisible() const noexcept
        {
            return false;
        }

        void SetVisible(bool){}


    public:
        Mouse* _owner;
        static Mouse::Impl* _s_mouse;
    };
    Mouse::Impl* Mouse::Impl::s_mouse = nullptr;
#else
    // ======================================================================================
    // Windows Store or Universal Windows Platform (UWP) app implementation
    // ======================================================================================
    //
    // For a Windows Store app or Universal Windows Platform (UWP) app, add the following to your existing
    // Application methods:
    //
    // Void App::SetWindow(CoreWindow^ window )
    // {
    // M_mouse->SetWindow(window);
    // }
    //
    // Void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
    // {
    // M_mouse->SetDpi(sender->LogicalDpi);
    // }
    //
    #include <Windows.Devices.Input.h>


    class Mouse::Impl
    {
    public:
        explicit Impl(Mouse* in_owner) noexcept (false) 
            : _state{}
            , _owner(in_owner)
            , _dpi(96.f)
            , _mode(MODE_ABSOLUTE)
            , _pointer_pressed_token{}
            , _pointer_released_token{}
            , _pointer_moved_token{}
            , _pointer_wheel_token{}
            , _pointer_mouse_moved_token{}
        {
            if (s_mouse)
            {
                throw std::exception("Mouse is a singleton");
            }
            s_mouse = this;
            _scroll_wheel_value.reset(CreateEventEx(
                nullptr,
                nullptr,
                CREATE_EVENT_MANUAL_RESET,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            _relative_read.reset(CreateEventEx(
                nullptr,
                nullptr,
                CREATE_EVENT_MANUAL_RESET,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            if (!_scroll_wheel_value || !_relative_read)
            {
                throw std::exception("CreateEventEx");
            }
        }

        ~Impl()
        {
            s_mouse = nullptr;
            RemoveHandlers();
        }

        void GetState(State&in_state) const
        {
            memcpy(
                &in_state,
                &_state,
                sizeof (State)
                );
            DWORD result = WaitForSingleObjectEx(
                _scroll_wheel_value.get(),
                0,
                FALSE
                );
            if (result == WAIT_FAILED) throw std::exception("WaitForSingleObjectEx");
            if (result == WAIT_OBJECT_0)
            {
                in_state._scroll_wheel_value = 0;
            }
            if (_mode == MODE_RELATIVE)
            {
                result = WaitForSingleObjectEx(
                    _relative_read.get(),
                    0,
                    FALSE
                    );
                if (result == WAIT_FAILED) throw std::exception("WaitForSingleObjectEx");
                if (result == WAIT_OBJECT_0)
                {
                    in_state._x = 0;
                    in_state._y = 0;
                }
                else
                {
                    SetEvent(_relative_read.get());
                }
            }
            in_state._position_mode = _mode;
        }

        void ResetScrollWheelValue() noexcept
        {
            SetEvent(_scroll_wheel_value.get());
        }

        void SetMode(Mode in_mode)
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::UI::Core;
            using namespace ABI::Windows::Foundation;
            if (_mode == in_mode) return;
            ComPtr < ICoreWindowStatic > statics;
            HRESULT hr = GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Core_CoreWindow) .Get(),
                statics.GetAddressOf()
                );
            ThrowIfFailed(hr);
            ComPtr < ICoreWindow > window;
            hr = statics->GetForCurrentThread(window.GetAddressOf());
            ThrowIfFailed(hr);
            if (in_mode == MODE_RELATIVE)
            {
                hr = window->get_pointer_cursor(_cursor.ReleaseAndGetAddressOf());
                ThrowIfFailed(hr);
                hr = window->put_pointer_cursor(nullptr);
                ThrowIfFailed(hr);
                SetEvent(_relative_read.get());
                _mode = MODE_RELATIVE;
            }
            else
            {
                if (!_cursor)
                {
                    ComPtr < ICoreCursorFactory > factory;
                    hr = GetActivationFactory(
                        HStringReference(RuntimeClass_Windows_UI_Core_CoreCursor) .Get(),
                        factory.GetAddressOf()
                        );
                    ThrowIfFailed(hr);
                    hr = factory->CreateCursor(
                        CoreCursorType_Arrow,
                        0,
                        _cursor.GetAddressOf()
                        );
                    ThrowIfFailed(hr);
                }
                hr = window->put_pointer_cursor(_cursor.Get());
                ThrowIfFailed(hr);
                _cursor.Reset();
                _mode = MODE_ABSOLUTE;
            }
        }

        bool IsConnected() const
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Devices::Input;
            using namespace ABI::Windows::Foundation;
            ComPtr < IMouseCapabilities > caps;
            HRESULT hr = RoActivateInstance(
                HStringReference(RuntimeClass_Windows_Devices_Input_MouseCapabilities) .Get(),
                &caps
                );
            ThrowIfFailed(hr);
            INT32 value;
            if (SUCCEEDED(caps->get_mouse_present(&value)))
            {
                return value != 0;
            }
            return false;
        }

        bool IsVisible() const noexcept
        {
            if (_mode == MODE_RELATIVE) return false;
            ComPtr < ABI::Windows::UI::Core::ICoreCursor > cursor;
            if (FAILED(_window->get_pointer_cursor(cursor.GetAddressOf()))) return false;
            return cursor != 0;
        }

        void SetVisible(bool in_visible)
        {
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Foundation;
            using namespace ABI::Windows::UI::Core;
            if (_mode == MODE_RELATIVE) return;
            if (in_visible)
            {
                if (!_cursor)
                {
                    ComPtr < ICoreCursorFactory > factory;
                    HRESULT hr = GetActivationFactory(
                        HStringReference(RuntimeClass_Windows_UI_Core_CoreCursor) .Get(),
                        factory.GetAddressOf()
                        );
                    ThrowIfFailed(hr);
                    hr = factory->CreateCursor(
                        CoreCursorType_Arrow,
                        0,
                        _cursor.GetAddressOf()
                        );
                    ThrowIfFailed(hr);
                }
                HRESULT hr = _window->put_pointer_cursor(_cursor.Get());
                ThrowIfFailed(hr);
            }
            else
            {
                HRESULT hr = _window->put_pointer_cursor(nullptr);
                ThrowIfFailed(hr);
            }
        }

        void SetWindow(ABI::Windows::UI::Core::ICoreWindow* in_window)
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Foundation;
            using namespace ABI::Windows::Devices::Input;
            if (_window.Get() == in_window) return;
            RemoveHandlers();
            _window = in_window;
            if (!in_window)
            {
                _cursor.Reset();
                _mouse.Reset();
                return;
            }
            ComPtr < IMouseDeviceStatics > mouse_statics;
            HRESULT hr = GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Devices_Input_MouseDevice) .Get(),
                mouse_statics.GetAddressOf()
                );
            ThrowIfFailed(hr);
            hr = mouse_statics->GetForCurrentView(_mouse.ReleaseAndGetAddressOf());
            ThrowIfFailed(hr);
            typedef \
                _fi_typed_event_handler_2_windows_c_devices_c_input_c_mouse_device_windows_c_devices_c_input_c_mouse_event_args\
            MouseMovedHandler;
            hr = _mouse->add_mouse_moved(
                Callback < MouseMovedHandler > (MouseMovedEvent) .Get(),
                &_pointer_mouse_moved_token
                );
            ThrowIfFailed(hr);
            typedef _fi_typed_event_handler_2_windows_cu_i_c_core_c_core_window_windows_cu_i_c_core_c_pointer_event_args\
            PointerHandler;
            auto cb = Callback < PointerHandler > (PointerEvent);
            hr = in_window->add_pointer_pressed(
                cb.Get(),
                &_pointer_pressed_token
                );
            ThrowIfFailed(hr);
            hr = in_window->add_pointer_released(
                cb.Get(),
                &_pointer_released_token
                );
            ThrowIfFailed(hr);
            hr = in_window->add_pointer_moved(
                cb.Get(),
                &_pointer_moved_token
                );
            ThrowIfFailed(hr);
            hr = in_window->add_pointer_wheel_changed(
                Callback < PointerHandler > (PointerWheel) .Get(),
                &_pointer_wheel_token
                );
            ThrowIfFailed(hr);
        }


    private:
        void RemoveHandlers()
        {
            if (_window)
            {
                (void)_window->remove_pointer_pressed(_pointer_pressed_token);
                _pointer_pressed_token._value = 0;
                (void)_window->remove_pointer_released(_pointer_released_token);
                _pointer_released_token._value = 0;
                (void)_window->remove_pointer_moved(_pointer_moved_token);
                _pointer_moved_token._value = 0;
                (void)_window->remove_pointer_wheel_changed(_pointer_wheel_token);
                _pointer_wheel_token._value = 0;
            }
            if (_mouse)
            {
                (void)_mouse->remove_mouse_moved(_pointer_mouse_moved_token);
                _pointer_mouse_moved_token._value = 0;
            }
        }

        static HRESULT PointerEvent(
            IInspectable*,
            ABI::Windows::UI::Core::IPointerEventArgs* in_args
            )
        {
            using namespace ABI::Windows::Foundation;
            using namespace ABI::Windows::UI::Input;
            using namespace ABI::Windows::Devices::Input;
            if (!s_mouse) return S_OK;
            ComPtr < IPointerPoint > current_point;
            HRESULT hr = in_args->get_current_point(current_point.GetAddressOf());
            ThrowIfFailed(hr);
            ComPtr < IPointerDevice > pointer_device;
            hr = current_point->get_pointer_device(pointer_device.GetAddressOf());
            ThrowIfFailed(hr);
            PointerDeviceType dev_type;
            hr = pointer_device->get_pointer_device_type(&dev_type);
            ThrowIfFailed(hr);
            if (dev_type == PointerDeviceType::PointerDeviceType_Mouse)
            {
                ComPtr < IPointerPointProperties > props;
                hr = current_point->get_properties(props.GetAddressOf());
                ThrowIfFailed(hr);
                boolean value;
                hr = props->get_is_left_button_pressed(&value);
                ThrowIfFailed(hr);
                s_mouse->_state._left_button = value != 0;
                hr = props->get_is_right_button_pressed(&value);
                ThrowIfFailed(hr);
                s_mouse->_state._right_button = value != 0;
                hr = props->get_is_middle_button_pressed(&value);
                ThrowIfFailed(hr);
                s_mouse->_state._middle_button = value != 0;
                hr = props->get_is_x_button1_pressed(&value);
                ThrowIfFailed(hr);
                s_mouse->_state._x_button1 = value != 0;
                hr = props->get_is_x_button2_pressed(&value);
                ThrowIfFailed(hr);
                s_mouse->_state._x_button2 = value != 0;
            }
            if (s_mouse->_mode == MODE_ABSOLUTE)
            {
                Point pos;
                hr = current_point->get_position(&pos);
                ThrowIfFailed(hr);
                float dpi = s_mouse->_dpi;
                s_mouse->_state._x = static_cast < int > (pos.X* dpi / 96.f + 0.5f);
                s_mouse->_state._y = static_cast < int > (pos.Y* dpi / 96.f + 0.5f);
            }
            return S_OK;
        }

        static HRESULT PointerWheel(
            IInspectable*,
            ABI::Windows::UI::Core::IPointerEventArgs* in_args
            )
        {
            using namespace ABI::Windows::Foundation;
            using namespace ABI::Windows::UI::Input;
            using namespace ABI::Windows::Devices::Input;
            if (!s_mouse) return S_OK;
            ComPtr < IPointerPoint > current_point;
            HRESULT hr = in_args->get_current_point(current_point.GetAddressOf());
            ThrowIfFailed(hr);
            ComPtr < IPointerDevice > pointer_device;
            hr = current_point->get_pointer_device(pointer_device.GetAddressOf());
            ThrowIfFailed(hr);
            PointerDeviceType dev_type;
            hr = pointer_device->get_pointer_device_type(&dev_type);
            ThrowIfFailed(hr);
            if (dev_type == PointerDeviceType::PointerDeviceType_Mouse)
            {
                ComPtr < IPointerPointProperties > props;
                hr = current_point->get_properties(props.GetAddressOf());
                ThrowIfFailed(hr);
                INT32 value;
                hr = props->get_mouse_wheel_delta(&value);
                ThrowIfFailed(hr);
                HANDLE evt = s_mouse->_scroll_wheel_value.get();
                if (WaitForSingleObjectEx(
                    evt,
                    0,
                    FALSE
                    ) == WAIT_OBJECT_0)
                {
                    s_mouse->_state._scroll_wheel_value = 0;
                    ResetEvent(evt);
                }
                s_mouse->_state._scroll_wheel_value += value;
                if (s_mouse->_mode == MODE_ABSOLUTE)
                {
                    Point pos;
                    hr = current_point->get_position(&pos);
                    ThrowIfFailed(hr);
                    float dpi = s_mouse->_dpi;
                    s_mouse->_state._x = static_cast < int > (pos.X* dpi / 96.f + 0.5f);
                    s_mouse->_state._y = static_cast < int > (pos.Y* dpi / 96.f + 0.5f);
                }
            }
            return S_OK;
        }

        static HRESULT MouseMovedEvent(
            IInspectable*,
            ABI::Windows::Devices::Input::IMouseEventArgs* in_args
            )
        {
            using namespace ABI::Windows::Devices::Input;
            if (!s_mouse) return S_OK;
            if (s_mouse->_mode == MODE_RELATIVE)
            {
                MouseDelta delta;
                HRESULT hr = in_args->get_mouse_delta(&delta);
                ThrowIfFailed(hr);
                s_mouse->_state._x = delta.X;
                s_mouse->_state._y = delta.Y;
                ResetEvent(s_mouse->_relative_read.get());
            }
            return S_OK;
        }


    public:
        State _state;
        Mouse* _owner;
        float _dpi;
        static Mouse::Impl* _s_mouse;

    private:
        Mode _mode;
        ComPtr < ABI::Windows::UI::Core::ICoreWindow > _window;
        ComPtr < ABI::Windows::Devices::Input::IMouseDevice > _mouse;
        ComPtr < ABI::Windows::UI::Core::ICoreCursor > _cursor;
        std::unique_ptr < void, HandleCloser > _scroll_wheel_value;
        std::unique_ptr < void, HandleCloser > _relative_read;
        EventRegistrationToken _pointer_pressed_token;
        EventRegistrationToken _pointer_released_token;
        EventRegistrationToken _pointer_moved_token;
        EventRegistrationToken _pointer_wheel_token;
        EventRegistrationToken _pointer_mouse_moved_token;
    };
    Mouse::Impl* Mouse::Impl::s_mouse = nullptr;
    void Mouse::SetWindow(ABI::Windows::UI::Core::ICoreWindow* in_window)
    {
        impl->SetWindow(in_window);
    }

    void Mouse::SetDpi(float in_dpi)
    {
        auto impl = Impl::s_mouse;
        if (!impl) return;
        impl->_dpi = in_dpi;
    }

#endif

#pragma warning(disable : 4355)
// Public constructor.
Mouse::Mouse() noexcept (false) 
    : _impl(std::make_unique < Impl > (this)){}

// Move constructor.
Mouse::Mouse(Mouse && in_move_from) noexcept 
    : _impl(std::move(in_move_from._impl))
{
    _impl->_owner = this;
}

// Move assignment.
Mouse&Mouse::operator=(Mouse && in_move_from) noexcept
{
    _impl = std::move(in_move_from._impl);
    _impl->_owner = this;
    return* this;
}

// Public destructor.
Mouse::~Mouse(){}

Mouse::State Mouse::GetState() const
{
    State state;
    _impl->GetState(state);
    return state;
}

void Mouse::ResetScrollWheelValue() noexcept
{
    _impl->ResetScrollWheelValue();
}

void Mouse::SetMode(Mode in_mode)
{
    _impl->SetMode(in_mode);
}

bool Mouse::IsConnected() const
{
    return _impl->IsConnected();
}

bool Mouse::IsVisible() const noexcept
{
    return _impl->IsVisible();
}

void Mouse::SetVisible(bool in_visible)
{
    _impl->SetVisible(in_visible);
}

Mouse&Mouse::Get()
{
    if (!Impl::s_mouse || !Impl::s_mouse->_owner) 
    {
        throw std::exception("Mouse is a singleton");
    }
    return* Impl::s_mouse->_owner;
}

// ======================================================================================
// ButtonStateTracker
// ======================================================================================
#define UPDATE_BUTTON_STATE(IN_FIELD) IN_FIELD = static_cast < ButtonState > ((!!in_state.IN_FIELD) | ((!!in_state.IN_FIELD ^ !!_last_state.\
    IN_FIELD) << 1));
void MouseButtonStateTracker::Update(const Mouse::State& in_state) noexcept
{
    UPDATE_BUTTON_STATE(_left_button) 
    
    assert((!in_state._left_button && !_last_state._left_button) == (\
        _left_button == UP));
    assert((in_state._left_button && _last_state._left_button) == (_left_button == HELD));
    assert((!in_state._left_button && _last_state._left_button) == (_left_button == RELEASED));
    assert((in_state._left_button && !_last_state._left_button) == (_left_button == PRESSED));

    UPDATE_BUTTON_STATE(_middle_button) 
    UPDATE_BUTTON_STATE(_right_button) 
    UPDATE_BUTTON_STATE(_x_button1)
    UPDATE_BUTTON_STATE(_x_button2) 

    _d_x = _last_state._x - in_state._x;
    _d_y = _last_state._y - in_state._y;
    _last_state = in_state;
}
#undef UPDATE_BUTTON_STATE

void MouseButtonStateTracker::Reset() noexcept
{
    memset(
        this,
        0,
        sizeof (MouseButtonStateTracker)
        );
}

