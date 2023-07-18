// --------------------------------------------------------------------------------------
// File: Mouse.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkId=248929
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#pragma once
#include <memory>

#if(defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)) || (defined (_XBOX_ONE) && defined (_TITLE) && (\
    _XDK_VER >= 0x42D907D1))
    namespace ABI
    {
        namespace Windows
        {
            namespace UI
            {
                namespace Core
                {
                    struct ICoreWindow;

                }
            }
        }
    }
#endif

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

namespace DirectX
{
    class Mouse
    {
    public:
        enum Mode
        {
            MODE_ABSOLUTE = 0,
            MODE_RELATIVE,
        };
        struct State
        {
        public:
            bool _left_button;
            bool _middle_button;
            bool _right_button;
            bool _x_button1;
            bool _x_button2;
            int _x;
            int _y;
            int _scroll_wheel_value;
            Mode _position_mode;
        };
        Mouse() noexcept (false);
        Mouse(Mouse && in_move_from) noexcept;
        Mouse& operator=(Mouse && in_move_from) noexcept;
        Mouse(Mouse const&) = delete;
        Mouse& operator=(Mouse const&) = delete;
        // Singleton
        static Mouse&__cdecl Get();
        virtual ~Mouse();
        // Retrieve the current state of the mouse
        State __cdecl GetState() const;
        // Resets the accumulated scroll wheel value
        void __cdecl ResetScrollWheelValue() noexcept;
        // Sets mouse mode (defaults to absolute)
        void __cdecl SetMode(Mode in_mode);
        // Feature detection
        bool __cdecl IsConnected() const;
        // Cursor visibility
        bool __cdecl IsVisible() const noexcept;
        void __cdecl SetVisible(bool in_visible);

    private:
        // Private implementation.
        class Impl;


    private:
        std::unique_ptr < Impl > _impl;
#ifdef WM_USER
#if !defined (WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
                void __cdecl SetWindow(HWND in_window);
                static void __cdecl ProcessMessage(
                    UINT in_message,
                    WPARAM in_w_param,
                    LPARAM in_l_param
                    );
#elif(WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
                static void __cdecl ProcessMessage(
                    UINT in_message,
                    WPARAM in_w_param,
                    LPARAM in_l_param
                    );
                static void __cdecl SetResolution(bool in_use4k);
#endif

#endif

#if(defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)) || (defined (_XBOX_ONE) && defined (_TITLE) && (\
    _XDK_VER >= 0x42D907D1))
            void __cdecl SetWindow(ABI::Windows::UI::Core::ICoreWindow* in_window);
#ifdef __cplusplus_winrt
                void __cdecl SetWindow(Windows::UI::Core::CoreWindow^ in_window)
                {
                    // See https://msdn.microsoft.com/en-us/library/hh755802.aspx
                    SetWindow(reinterpret_cast < ABI::Windows::UI::Core::ICoreWindow* > (in_window));
                }

#endif

#ifdef CPPWINRT_VERSION
                void __cdecl SetWindow(in_winrt::Windows::UI::Core::CoreWindow in_window)
                {
                    // See https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/interop-winrt-abi
                    SetWindow(reinterpret_cast < ABI::Windows::UI::Core::ICoreWindow* > (in_winrt::get_abi(in_window)));
                }

#endif

            static void __cdecl SetDpi(float in_dpi);
#endif
// WINAPI_FAMILY == WINAPI_FAMILY_APP

    };
    class MouseButtonStateTracker
    {
    public:
        enum ButtonState
        {
            UP = 0,
            // Button is up
            HELD = 1,
            // Button is held down
            RELEASED = 2,
            // Button was just released
            PRESSED = 3,
            // Buton was just pressed
        };
        #pragma prefast(suppress : 26495, "Reset() performs the initialization")
        MouseButtonStateTracker() noexcept
        {
            Reset();
        }

        void __cdecl Update(const Mouse::State& in_state) noexcept;
        void __cdecl Reset() noexcept;
        Mouse::State __cdecl GetLastState() const noexcept
        {
            return _last_state;
        }

    public:
        ButtonState _left_button;
        ButtonState _middle_button;
        ButtonState _right_button;
        ButtonState _x_button1;
        ButtonState _x_button2;
        int _d_x;
        int _d_y;

    private:
        Mouse::State _last_state;
    };
}
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

