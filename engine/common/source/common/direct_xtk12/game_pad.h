// --------------------------------------------------------------------------------------
// File: GamePad.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkId=248929
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#pragma once
#if(_WIN32_WINNT < 0x0A00 /* _WIN32_WINNT_WIN10 */
) || defined (_GAMING_DESKTOP)
#ifndef _XBOX_ONE
#if !defined (WINAPI_FAMILY) || (WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP)
#if(_WIN32_WINNT >= 0x0602 /* _WIN32_WINNT_WIN8 */
)
                #pragma comment(lib, "xinput.lib")
#else
                #pragma comment(lib, "xinput9_1_0.lib")
#endif

#endif

#endif

#endif

#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
    interface IGameInputDevice;
#endif

#include <cstdint>
#include <memory>

#if(_WIN32_WINNT >= 0x0A00 /* _WIN32_WINNT_WIN10 */
) && !defined (_GAMING_DESKTOP)
    #pragma comment(lib, "runtimeobject.lib")
    #include <string>


#endif

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

namespace DirectX
{
    class GamePad
    {
    public:
        enum DeadZone
        {
            DEAD_ZONE_INDEPENDENT_AXES = 0,
            DEAD_ZONE_CIRCULAR,
            DEAD_ZONE_NONE,
        };
        struct Buttons
        {
        public:
            bool _a;
            bool _b;
            bool _x;
            bool _y;
            bool _left_stick;
            bool _right_stick;
            bool _left_shoulder;
            bool _right_shoulder;
            union
            {
                bool back;
                bool view;
            };
            union
            {
                bool start;
                bool menu;
            };
        };
        struct DPad
        {
        public:
            bool _up;
            bool _down;
            bool _right;
            bool _left;
        };
        struct ThumbSticks
        {
        public:
            float _left_x;
            float _left_y;
            float _right_x;
            float _right_y;
        };
        struct Triggers
        {
        public:
            float _left;
            float _right;
        };
        struct State
        {
        public:
            bool __cdecl IsConnected() const noexcept
            {
                return connected;
            }

            // Is the button pressed currently?
            bool __cdecl IsAPressed() const noexcept
            {
                return buttons._a;
            }

            bool __cdecl IsBPressed() const noexcept
            {
                return buttons._b;
            }

            bool __cdecl IsXPressed() const noexcept
            {
                return buttons._x;
            }

            bool __cdecl IsYPressed() const noexcept
            {
                return buttons._y;
            }

            bool __cdecl IsLeftStickPressed() const noexcept
            {
                return buttons._left_stick;
            }

            bool __cdecl IsRightStickPressed() const noexcept
            {
                return buttons._right_stick;
            }

            bool __cdecl IsLeftShoulderPressed() const noexcept
            {
                return buttons._left_shoulder;
            }

            bool __cdecl IsRightShoulderPressed() const noexcept
            {
                return buttons._right_shoulder;
            }

            bool __cdecl IsBackPressed() const noexcept
            {
                return buttons._back;
            }

            bool __cdecl IsViewPressed() const noexcept
            {
                return buttons._view;
            }

            bool __cdecl IsStartPressed() const noexcept
            {
                return buttons._start;
            }

            bool __cdecl IsMenuPressed() const noexcept
            {
                return buttons._menu;
            }

            bool __cdecl IsDPadDownPressed() const noexcept
            {
                return dpad._down;
            }

            bool __cdecl IsDPadUpPressed() const noexcept
            {
                return dpad._up;
            }

            bool __cdecl IsDPadLeftPressed() const noexcept
            {
                return dpad._left;
            }

            bool __cdecl IsDPadRightPressed() const noexcept
            {
                return dpad._right;
            }

            bool __cdecl IsLeftThumbStickUp() const noexcept
            {
                return (in_thumb_sticks._left_y > 0.5f) != 0;
            }

            bool __cdecl IsLeftThumbStickDown() const noexcept
            {
                return (in_thumb_sticks._left_y < - 0.5f) != 0;
            }

            bool __cdecl IsLeftThumbStickLeft() const noexcept
            {
                return (in_thumb_sticks._left_x < - 0.5f) != 0;
            }

            bool __cdecl IsLeftThumbStickRight() const noexcept
            {
                return (in_thumb_sticks._left_x > 0.5f) != 0;
            }

            bool __cdecl IsRightThumbStickUp() const noexcept
            {
                return (in_thumb_sticks._right_y > 0.5f) != 0;
            }

            bool __cdecl IsRightThumbStickDown() const noexcept
            {
                return (in_thumb_sticks._right_y < - 0.5f) != 0;
            }

            bool __cdecl IsRightThumbStickLeft() const noexcept
            {
                return (in_thumb_sticks._right_x < - 0.5f) != 0;
            }

            bool __cdecl IsRightThumbStickRight() const noexcept
            {
                return (in_thumb_sticks._right_x > 0.5f) != 0;
            }

            bool __cdecl IsLeftTriggerPressed() const noexcept
            {
                return (in_triggers._left > 0.5f) != 0;
            }

            bool __cdecl IsRightTriggerPressed() const noexcept
            {
                return (in_triggers._right > 0.5f) != 0;
            }


        public:
            bool _connected;
            uint64_t _packet;
            Buttons _buttons;
            DPad _dpad;
            ThumbSticks _thumb_sticks;
            Triggers _triggers;
        };
        struct Capabilities
        {
        public:
            enum Type
            {
                UNKNOWN = 0,
                GAMEPAD,
                WHEEL,
                ARCADE_STICK,
                FLIGHT_STICK,
                DANCE_PAD,
                GUITAR,
                GUITAR_ALTERNATE,
                DRUM_KIT,
                GUITAR_BASS = 11,
                ARCADE_PAD = 19,
            };
            Capabilities() noexcept 
                : _connected(false)
                , _gamepad_type(UNKNOWN)
                , _id{}
                , _vid(0)
                , _pid(0) _{} bool __cdecl IsConnected() const noexcept
            {
                return connected;
            }


        public:
            bool _connected;
            Type _gamepad_type;
            uint16_t _vid;
            uint16_t _pid;
#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
                APP_LOCAL_DEVICE_ID _id;
#elif(_WIN32_WINNT >= 0x0A00 /* _WIN32_WINNT_WIN10 */
) && !defined (_GAMING_DESKTOP)
                std::_wstring _id;
#else
                uint64_t _id;
#endif

        };
        GamePad() noexcept (false);
        GamePad(GamePad && in_move_from) noexcept;
        GamePad&_operator=(GamePad && in_move_from) noexcept;
        GamePad(GamePad const&) = delete;
        GamePad&_operator=(GamePad const&) = delete;
        // Singleton
        static GamePad&__cdecl Get();
        virtual ~GamePad();
        // Retrieve the current state of the gamepad of the associated player index
        State __cdecl GetState(
            int in_player,
            DeadZone in_dead_zone_mode = DEAD_ZONE_INDEPENDENT_AXES
            );
        // Retrieve the current capabilities of the gamepad of the associated player index
        Capabilities __cdecl GetCapabilities(int in_player);
        // Set the vibration motor speeds of the gamepad
        bool __cdecl SetVibration(
            int in_player,
            float in_left_motor,
            float in_right_motor,
            float in_left_trigger = 0.f,
            float in_right_trigger = 0.f
            ) noexcept;
        // Handle suspending/resuming
        void __cdecl Suspend() noexcept;
        void __cdecl Resume() noexcept;

    private:
        // Private implementation.
        class Impl;


    public:
        static constexpr int c_most_recent = - 1;

    private:
        std::_unique_ptr < Impl > _impl;
#if((_WIN32_WINNT >= 0x0A00 /* _WIN32_WINNT_WIN10 */
) && !defined (_GAMING_DESKTOP)) || defined (_XBOX_ONE)
            static constexpr int MAX_PLAYER_COUNT = 8;
#else
            static constexpr int MAX_PLAYER_COUNT = 4;
#endif

#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
            static constexpr int c_merged_input = - 2;
#endif

#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
            void __cdecl RegisterEvents(void* in_ctrl_changed) noexcept;
#elif((_WIN32_WINNT >= 0x0A00 /* _WIN32_WINNT_WIN10 */
) && !defined (_GAMING_DESKTOP)) || defined (_XBOX_ONE)
            void __cdecl RegisterEvents(
                void* in_ctrl_changed,
                void* in_user_changed
                ) noexcept;
#endif

#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
            // Underlying device access
            void __cdecl GetDevice(
                int in_player,
                in_outptr_ IGameInputDevice** in_device
                ) noexcept;
#endif

    };
    class GamePadButtonStateTracker
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
        GamePadButtonStateTracker() noexcept
        {
            Reset();
        }

        void __cdecl Update(const GamePad::State&in_state) noexcept;
        void __cdecl Reset() noexcept;
        GamePad::State __cdecl GetLastState() const noexcept
        {
            return last_state;
        }


    public:
        ButtonState _a;
        ButtonState _b;
        ButtonState _x;
        ButtonState _y;
        ButtonState _left_stick;
        ButtonState _right_stick;
        ButtonState _left_shoulder;
        ButtonState _right_shoulder;
        union
        {
            ButtonState back;
            ButtonState view;
        };
        union
        {
            ButtonState start;
            ButtonState menu;
        };
        ButtonState _dpad_up;
        ButtonState _dpad_down;
        ButtonState _dpad_left;
        ButtonState _dpad_right;
        ButtonState _left_stick_up;
        ButtonState _left_stick_down;
        ButtonState _left_stick_left;
        ButtonState _left_stick_right;
        ButtonState _right_stick_up;
        ButtonState _right_stick_down;
        ButtonState _right_stick_left;
        ButtonState _right_stick_right;
        ButtonState _left_trigger;
        ButtonState _right_trigger;

    private:
        GamePad::State _last_state;
    };
}
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

