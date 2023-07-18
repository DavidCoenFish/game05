// --------------------------------------------------------------------------------------
// File: GamePad.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkId=248929
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#include "common/common_pch.h"

#include "common/direct_xtk12/game_pad.h"
using namespace DirectX;
using Microsoft::WRL::ComPtr;
namespace
{
    constexpr float c_xbox_one_thumb_dead_zone = .24f;
    // Recommended Xbox One controller deadzone
    float ApplyLinearDeadZone(
        float in_value,
        float in_max_value,
        float in_dead_zone_size
        ) noexcept
    {
        if (in_value < - in_dead_zone_size)
        {
            // Increase negative values to remove the deadzone discontinuity.
            in_value += in_dead_zone_size;
        }
        else if (in_value > in_dead_zone_size)
        {
            // Decrease positive values to remove the deadzone discontinuity.
            in_value -= in_dead_zone_size;
        }

        else
        {
            // Values inside the deadzone come out zero.
            return 0;
        }
        // Scale into 0-1 range.
        float scaled_value = in_value / (in_max_value - in_dead_zone_size);
        return std::max(
            - 1.f,
            std::min(
                scaled_value,
                1.f
                )
            );
    }

    void ApplyStickDeadZone(
        float in_x,
        float in_y,
        GamePad::DeadZone in_dead_zone_mode,
        float in_max_value,
        float in_dead_zone_size,
        _Out_ float& in_result_x,
        _Out_ float& in_result_y
        ) noexcept
    {
        switch (in_dead_zone_mode)
        {
        case GamePad::DEAD_ZONE_INDEPENDENT_AXES:
            in_result_x = ApplyLinearDeadZone(
                in_x,
                in_max_value,
                in_dead_zone_size
                );
            in_result_y = ApplyLinearDeadZone(
                in_y,
                in_max_value,
                in_dead_zone_size
                );
            break;
        case GamePad::DEAD_ZONE_CIRCULAR:

            {
                float dist = sqrtf(in_x* in_x + in_y* in_y);
                float wanted = ApplyLinearDeadZone(
                    dist,
                    in_max_value,
                    in_dead_zone_size
                    );
                float scale = (wanted > 0.f) ? (wanted / dist) : 0.f;
                in_result_x = std::max(
                    - 1.f,
                    std::min(
                        in_x* scale,
                        1.f
                        )
                    );
                in_result_y = std::max(
                    - 1.f,
                    std::min(
                        in_y* scale,
                        1.f
                        )
                    );
            }
            break;
        default:
            // GamePad::DEAD_ZONE_NONE
            in_result_x = ApplyLinearDeadZone(
                in_x,
                in_max_value,
                0
                );
            in_result_y = ApplyLinearDeadZone(
                in_y,
                in_max_value,
                0
                );
            break;
        }

    }

}
#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
    #include <GameInput.h>

    // ======================================================================================
    // GameInput
    // ======================================================================================
    class GamePad::Impl
    {
    public:
        Impl(GamePad* in_owner) 
            : _owner(in_owner)
            , _ctrl_changed(INVALID_HANDLE_VALUE)
            , _device_token(0)
            , _most_recent_gamepad(0)
        {
            if (s_game_pad)
            {
                throw std::exception("GamePad is a singleton");
            }
            s_game_pad = this;
            ThrowIfFailed(GameInputCreate(_game_input.GetAddressOf()));
            ThrowIfFailed(_game_input->RegisterDeviceCallback(
                nullptr,
                GameInputKindGamepad,
                GameInputDeviceConnected,
                GameInputBlockingEnumeration,
                this,
                OnGameInputDevice,
                &_device_token
                ));
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
                            "ERROR: GameInput::UnregisterCallback [gamepad] failed (%08X)",
                            static_cast < unsigned int > (hr)
                            );
                    }
                }
                _device_token = 0;
            }
            s_game_pad = nullptr;
        }

        void GetState(
            int in_player,
            _Out_ State&in_state,
            DeadZone in_dead_zone_mode
            )
        {
            memset(
                &in_state,
                0,
                sizeof (State)
                );
            IGameInputDevice* device = nullptr;
            if (in_player >= 0 && in_player < MAX_PLAYER_COUNT)
            {
                device = _input_devices[in_player].Get();
                if (!device) return;
            }
            else if (in_player == c_most_recent)
            {
                in_player = _most_recent_gamepad;
                assert(in_player >= 0 && in_player < MAX_PLAYER_COUNT);
                device = _input_devices[in_player].Get();
                if (!device) return;
            }

            else if (in_player != c_merged_input)
            {
                return;
            }

            ComPtr < IGameInputReading > reading;
            if (SUCCEEDED(_game_input->GetCurrentReading(
                GameInputKindGamepad,
                device,
                reading.GetAddressOf()
                )))
            {
                GameInputGamepadState pad;
                if (reading->GetGamepadState(&pad))
                {
                    state._connected = true;
                    state._packet = reading->GetSequenceNumber(GameInputKindGamepad);
                    state._buttons._a = (pad._buttons&GameInputGamepadA) != 0;
                    state._buttons._b = (pad._buttons&GameInputGamepadB) != 0;
                    state._buttons._x = (pad._buttons&GameInputGamepadX) != 0;
                    state._buttons._y = (pad._buttons&GameInputGamepadY) != 0;
                    state._buttons._left_stick = (pad._buttons&GameInputGamepadLeftThumbstick) != 0;
                    state._buttons._right_stick = (pad._buttons&GameInputGamepadRightThumbstick) != 0;
                    state._buttons._left_shoulder = (pad._buttons&GameInputGamepadLeftShoulder) != 0;
                    state._buttons._right_shoulder = (pad._buttons&GameInputGamepadRightShoulder) != 0;
                    state._buttons._view = (pad._buttons&GameInputGamepadView) != 0;
                    state._buttons._menu = (pad._buttons&GameInputGamepadMenu) != 0;
                    state._dpad._up = (pad._buttons&GameInputGamepadDPadUp) != 0;
                    state._dpad._down = (pad._buttons&GameInputGamepadDPadDown) != 0;
                    state._dpad._right = (pad._buttons&GameInputGamepadDPadRight) != 0;
                    state._dpad._left = (pad._buttons&GameInputGamepadDPadLeft) != 0;
                    ApplyStickDeadZone(
                        pad._left_thumbstick_x,
                        pad._left_thumbstick_y,
                        in_dead_zone_mode,
                        1.f,
                        c_xbox_one_thumb_dead_zone,
                        state._thumb_sticks._left_x,
                        state._thumb_sticks._left_y
                        );
                    ApplyStickDeadZone(
                        pad._right_thumbstick_x,
                        pad._right_thumbstick_y,
                        in_dead_zone_mode,
                        1.f,
                        c_xbox_one_thumb_dead_zone,
                        state._thumb_sticks._right_x,
                        state._thumb_sticks._right_y
                        );
                    state._triggers._left = pad._left_trigger;
                    state._triggers._right = pad._right_trigger;
                }
            }
        }

        void GetCapabilities(
            int in_player,
            _Out_ Capabilities&in_caps
            )
        {
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if (in_player >= 0 && in_player < MAX_PLAYER_COUNT)
            {
                IGameInputDevice* device = _input_devices[in_player].Get();
                if (device)
                {
                    if (device->GetDeviceStatus() &GameInputDeviceConnected)
                    {
                        auto device_info = device->GetDeviceInfo();
                        in_caps._connected = true;
                        in_caps._gamepad_type = Capabilities::GAMEPAD;
                        in_caps._id = device_info->_device_id;
                        in_caps._vid = device_info->_vendor_id;
                        in_caps._pid = device_info->_product_id;
                        return;
                    }
                    else
                    {
                        _input_devices[in_player].Reset();
                    }
                }
            }
            memset(
                &in_caps,
                0,
                sizeof (Capabilities)
                );
        }

        bool SetVibration(
            int in_player,
            float in_left_motor,
            float in_right_motor,
            float in_left_trigger,
            float in_right_trigger
            ) noexcept
        {
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if (in_player >= 0 && in_player < MAX_PLAYER_COUNT)
            {
                IGameInputDevice* device = _input_devices[in_player].Get();
                if (device)
                {
                    GameInputRumbleParams const params =
                    {
                        in_left_motor, in_right_motor, in_left_trigger, in_right_trigger};
                    device->SetRumbleState(&params);
                    return true;
                }
            }
            return false;
        }

        void Suspend() noexcept
        {
            for (int player = 0; player < MAX_PLAYER_COUNT;++ player)
            {
                IGameInputDevice* device = _input_devices[player].Get();
                if (device)
                {
                    device->SetRumbleState(nullptr);
                }
            }
        }

        void Resume() noexcept
        {
            for (int player = 0; player < MAX_PLAYER_COUNT;++ player)
            {
                IGameInputDevice* device = _input_devices[player].Get();
                if (device)
                {
                    if (! (device->GetDeviceStatus() &GameInputDeviceConnected))
                    {
                        _input_devices[player].Reset();
                    }
                }
            }
        }

        void GetDevice(
            int in_player,
            in_outptr_ IGameInputDevice** in_device
            ) noexcept
        {
            if (!in_device) return;
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            * in_device = nullptr;
            if (in_player >= 0 && in_player < MAX_PLAYER_COUNT)
            {
                IGameInputDevice* dev = _input_devices[in_player].Get();
                if (dev)
                {
                    dev->AddRef();
                    * in_device = dev;
                }
            }
        }


    private:
        static void CALLBACK OnGameInputDevice(
            _In_ GameInputCallbackToken,
            _In_ void* in_context,
            _In_ IGameInputDevice* in_device,
            _In_ uint64_t,
            _In_ GameInputDeviceStatus in_current_status,
            _In_ GameInputDeviceStatus
            ) noexcept
        {
            auto impl = reinterpret_cast < GamePad::Impl* > (in_context);
            if (in_current_status&GameInputDeviceConnected)
            {
                size_t empty = MAX_PLAYER_COUNT;
                size_t k = 0;
                for (; k < MAX_PLAYER_COUNT;++ k)
                {
                    if (impl->_input_devices[k].Get() == in_device)
                    {
                        impl->_most_recent_gamepad = static_cast < int > (k);
                        break;
                    }
                    else if (!impl->_input_devices[k])
                    {
                        if (empty >= MAX_PLAYER_COUNT) empty = k;
                    }

                }
                if (k >= MAX_PLAYER_COUNT)
                {
                    // Silently ignore "extra" gamepads as there's no hard limit
                    if (empty < MAX_PLAYER_COUNT)
                    {
                        impl->_input_devices[empty] = device;
                        impl->_most_recent_gamepad = static_cast < int > (empty);
                    }
                }
            }
            else
            {
                for (size_t k = 0; k < MAX_PLAYER_COUNT;++ k)
                {
                    if (impl->_input_devices[k].Get() == device)
                    {
                        impl->_input_devices[k].Reset();
                        break;
                    }
                }
            }
            if (impl->_ctrl_changed != INVALID_HANDLE_VALUE)
            {
                SetEvent(impl->_ctrl_changed);
            }
        }


    public:
        GamePad* _owner;
        static GamePad::Impl* _s_game_pad;
        HANDLE _ctrl_changed;

    private:
        ComPtr < IGameInput > _game_input;
        ComPtr < IGameInputDevice > _input_devices[MAX_PLAYER_COUNT];
        GameInputCallbackToken _device_token;
        int _most_recent_gamepad;
    };
    GamePad::Impl* GamePad::Impl::s_game_pad = nullptr;
#elif(_WIN32_WINNT >= _WIN32_WINNT_WIN10) && !defined (_GAMING_DESKTOP)
    // ======================================================================================
    // Windows::Gaming::Input (Windows 10)
    // ======================================================================================
    #pragma warning(push)
    #pragma warning(disable : 4471 5204)
    #include <windows.gaming.input.h>

    #pragma warning(pop)
    class GamePad::Impl
    {
    public:
        Impl(GamePad* in_owner) 
            : _owner(in_owner)
            , _ctrl_changed(INVALID_HANDLE_VALUE)
            , _user_changed(INVALID_HANDLE_VALUE)
            , _most_recent_gamepad(0)
            , _statics{}
            , _game_pad{}
            , _user_change_token{}
            , _added_token{}
            , _removed_token{}
            , _changed{}
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Foundation;
            if (s_game_pad)
            {
                throw std::exception("GamePad is a singleton");
            }
            s_game_pad = this;
            _changed.reset(CreateEventEx(
                nullptr,
                nullptr,
                0,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            if (!_changed)
            {
                throw std::exception("CreateEventEx");
            }
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Gaming_Input_Gamepad) .Get(),
                _statics.GetAddressOf()
                ));
            typedef _fi_event_handler_1_windows_c_gaming_c_input_c_gamepad AddedHandler;
            ThrowIfFailed(_statics->in_add_gamepad_added(
                Callback < AddedHandler > (GamepadAdded) .Get(),
                &_added_token
                ));
            typedef _fi_event_handler_1_windows_c_gaming_c_input_c_gamepad RemovedHandler;
            ThrowIfFailed(_statics->in_add_gamepad_removed(
                Callback < RemovedHandler > (GamepadRemoved) .Get(),
                &_removed_token
                ));
            ScanGamePads();
        }

        Impl(Impl &&) = default;
        Impl& operator=(Impl &&) = default;
        Impl(Impl const&) = delete;
        Impl& operator=(Impl const&) = delete;
        ~Impl()
        {
            using namespace ABI::Windows::Gaming::Input;
            for (size_t j = 0; j < MAX_PLAYER_COUNT;++ j)
            {
                if (_game_pad[j])
                {
                    ComPtr < IGameController > ctrl;
                    HRESULT hr = _game_pad[j].As(&ctrl);
                    if (SUCCEEDED(hr) && ctrl)
                    {
                        (void)ctrl->remove_user_changed(_user_change_token[j]);
                        _user_change_token[j]._value = 0;
                    }
                    _game_pad[j].Reset();
                }
            }
            if (_statics)
            {
                (void)_statics->remove_gamepad_added(_added_token);
                _added_token._value = 0;
                (void)_statics->remove_gamepad_removed(_removed_token);
                _removed_token._value = 0;
                _statics.Reset();
            }
            s_game_pad = nullptr;
        }

        void GetState(
            int in_player,
            _Out_ State&in_state,
            DeadZone in_dead_zone_mode
            )
        {
            using namespace Microsoft::WRL;
            using namespace ABI::Windows::Gaming::Input;
            if (WaitForSingleObjectEx(
                _changed.get(),
                0,
                FALSE
                ) == WAIT_OBJECT_0)
            {
                ScanGamePads();
            }
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if ((in_player >= 0) && (in_player < MAX_PLAYER_COUNT))
            {
                if (_game_pad[in_player])
                {
                    GamepadReading reading;
                    HRESULT hr = _game_pad[in_player]->GetCurrentReading(&reading);
                    if (SUCCEEDED(hr))
                    {
                        in_state._connected = true;
                        in_state._packet = reading.Timestamp;
                        in_state._buttons._a = (reading.Buttons&GamepadButtons::GamepadButtons_A) != 0;
                        in_state._buttons._b = (reading.Buttons&GamepadButtons::GamepadButtons_B) != 0;
                        in_state._buttons._x = (reading.Buttons&GamepadButtons::GamepadButtons_X) != 0;
                        in_state._buttons._y = (reading.Buttons&GamepadButtons::GamepadButtons_Y) != 0;
                        in_state._buttons._left_stick = (reading.Buttons&GamepadButtons::GamepadButtons_LeftThumbstick) \
                            != 0;
                        in_state._buttons._right_stick = (reading.Buttons&GamepadButtons::GamepadButtons_RightThumbstick\
                            ) != 0;
                        in_state._buttons._left_shoulder = (reading.Buttons&GamepadButtons::GamepadButtons_LeftShoulder)\
                        != 0;
                        in_state._buttons._right_shoulder = (reading.Buttons&GamepadButtons::\
                            GamepadButtons_RightShoulder) != 0;
                        in_state._buttons._back = (reading.Buttons&GamepadButtons::GamepadButtons_View) != 0;
                        in_state._buttons._start = (reading.Buttons&GamepadButtons::GamepadButtons_Menu) != 0;
                        in_state._dpad._up = (reading.Buttons&GamepadButtons::GamepadButtons_DPadUp) != 0;
                        in_state._dpad._down = (reading.Buttons&GamepadButtons::GamepadButtons_DPadDown) != 0;
                        in_state._dpad._right = (reading.Buttons&GamepadButtons::GamepadButtons_DPadRight) != 0;
                        in_state._dpad._left = (reading.Buttons&GamepadButtons::GamepadButtons_DPadLeft) != 0;
                        ApplyStickDeadZone(
                            static_cast < float > (reading.LeftThumbstickX),
                            static_cast < float > (reading.LeftThumbstickY),
                            in_dead_zone_mode,
                            1.f,
                            c_xbox_one_thumb_dead_zone,
                            in_state._thumb_sticks._left_x,
                            in_state._thumb_sticks._left_y
                            );
                        ApplyStickDeadZone(
                            static_cast < float > (reading.RightThumbstickX),
                            static_cast < float > (reading.RightThumbstickY),
                            in_dead_zone_mode,
                            1.f,
                            c_xbox_one_thumb_dead_zone,
                            in_state._thumb_sticks._right_x,
                            in_state._thumb_sticks._right_y
                            );
                        in_state._triggers._left = static_cast < float > (reading.LeftTrigger);
                        in_state._triggers._right = static_cast < float > (reading.RightTrigger);
                        return;
                    }
                }
            }
            memset(
                &in_state,
                0,
                sizeof (State)
                );
        }

        void GetCapabilities(
            int in_player,
            Capabilities&in_caps
            )
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Foundation;
            using namespace ABI::Windows::System;
            using namespace ABI::Windows::Gaming::Input;
            if (WaitForSingleObjectEx(
                _changed.get(),
                0,
                FALSE
                ) == WAIT_OBJECT_0)
            {
                ScanGamePads();
            }
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if ((in_player >= 0) && (in_player < MAX_PLAYER_COUNT))
            {
                if (_game_pad[in_player])
                {
                    in_caps._connected = true;
                    in_caps._gamepad_type = Capabilities::GAMEPAD;
                    in_caps._id.clear();
                    in_caps._vid = in_caps._pid = 0;
                    ComPtr < IGameController > ctrl;
                    HRESULT hr = _game_pad[in_player].As(&ctrl);
                    if (SUCCEEDED(hr) && ctrl)
                    {
                        ComPtr < IUser > user;
                        hr = ctrl->get_user(user.GetAddressOf());
                        if (SUCCEEDED(hr) && user != nullptr)
                        {
                            HString str;
                            hr = user->get_non_roamable_id(str.GetAddressOf());
                            if (SUCCEEDED(hr))
                            {
                                in_caps._id = str.GetRawBuffer(nullptr);
                            }
                        }
                        // Requires the Windows 10 Creators Update SDK (15063)
#if defined (NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
                            ComPtr < IRawGameControllerStatics > raw_statics;
                            hr = GetActivationFactory(
                                HStringReference(RuntimeClass_Windows_Gaming_Input_RawGameController) .Get(),
                                raw_statics.GetAddressOf()
                                );
                            if (SUCCEEDED(hr))
                            {
                                ComPtr < IRawGameController > raw;
                                hr = raw_statics->FromGameController(
                                    ctrl.Get(),
                                    raw.GetAddressOf()
                                    );
                                if (SUCCEEDED(hr) && raw)
                                {
                                    if (FAILED(raw->get_hardware_vendor_id(&in_caps._vid))) in_caps._vid = 0;
                                    if (FAILED(raw->get_hardware_product_id(&in_caps._pid))) in_caps._pid = 0;
                                }
                            }
#endif
// NTDDI_WIN10_RS2

                    }
                    return;
                }
            }
            in_caps._id.clear();
            in_caps = {};
        }

        bool SetVibration(
            int in_player,
            float in_left_motor,
            float in_right_motor,
            float in_left_trigger,
            float in_right_trigger
            ) noexcept
        {
            using namespace ABI::Windows::Gaming::Input;
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if ((in_player >= 0) && (in_player < MAX_PLAYER_COUNT))
            {
                if (_game_pad[in_player])
                {
                    GamepadVibration vib;
                    vib.LeftMotor = double (in_left_motor);
                    vib.RightMotor = double (in_right_motor);
                    vib.LeftTrigger = double (in_left_trigger);
                    vib.RightTrigger = double (in_right_trigger);
                    HRESULT hr = _game_pad[in_player]->put_vibration(vib);
                    if (SUCCEEDED(hr)) return true;
                }
            }
            return false;
        }

        void Suspend() noexcept
        {
            for (size_t j = 0; j < MAX_PLAYER_COUNT;++ j)
            {
                _game_pad[j].Reset();
            }
        }

        void Resume() noexcept
        {
            // Make sure we rescan gamepads
            SetEvent(_changed.in_get());
        }


    private:
        void ScanGamePads()
        {
            using namespace Microsoft::WRL;
            using namespace ABI::Windows::Foundation::Collections;
            using namespace ABI::Windows::Gaming::Input;
            ComPtr < IVectorView < Gamepad* >> pads;
            ThrowIfFailed(_statics->in_get_gamepads(pads.GetAddressOf()));
            unsigned int count = 0;
            ThrowIfFailed(pads->in_get_size(&count));
            // Check for removed gamepads
            for (size_t j = 0; j < MAX_PLAYER_COUNT;++ j)
            {
                if (_game_pad[j])
                {
                    unsigned int k = 0;
                    for (; k < count;++ k)
                    {
                        ComPtr < IGamepad > pad;
                        HRESULT hr = pads->GetAt(
                            k,
                            pad.GetAddressOf()
                            );
                        if (SUCCEEDED(hr) && (pad == _game_pad[j]))
                        {
                            break;
                        }
                    }
                    if (k >= count)
                    {
                        ComPtr < IGameController > ctrl;
                        HRESULT hr = _game_pad[j].As(&ctrl);
                        if (SUCCEEDED(hr) && ctrl)
                        {
                            (void)ctrl->remove_user_changed(_user_change_token[j]);
                            _user_change_token[j]._value = 0;
                        }
                        _game_pad[j].Reset();
                    }
                }
            }
            // Check for added gamepads
            for (unsigned int j = 0; j < count;++ j)
            {
                ComPtr < IGamepad > pad;
                HRESULT hr = pads->GetAt(
                    j,
                    pad.GetAddressOf()
                    );
                if (SUCCEEDED(hr))
                {
                    size_t empty = MAX_PLAYER_COUNT;
                    size_t k = 0;
                    for (; k < MAX_PLAYER_COUNT;++ k)
                    {
                        if (_game_pad[k] == pad)
                        {
                            if (j == (count - 1)) _most_recent_gamepad = static_cast < int > (k);
                            break;
                        }
                        else if (!_game_pad[k])
                        {
                            if (empty >= MAX_PLAYER_COUNT) empty = k;
                        }

                    }
                    if (k >= MAX_PLAYER_COUNT)
                    {
                        // Silently ignore "extra" gamepads as there's no hard limit
                        if (empty < MAX_PLAYER_COUNT)
                        {
                            _game_pad[empty] = pad;
                            if (j == (count - 1)) _most_recent_gamepad = static_cast < int > (empty);
                            ComPtr < IGameController > ctrl;
                            hr = pad.As(&ctrl);
                            if (SUCCEEDED(hr) && ctrl)
                            {
                                typedef \
                                    _fi_typed_event_handler_2_windows_c_gaming_c_input_ci_game_controller_windows_c_system_c_user_changed_event_args\
                                UserHandler;
                                ThrowIfFailed(ctrl->in_add_user_changed(
                                    Callback < UserHandler > (UserChanged) .Get(),
                                    &_user_change_token[empty]
                                    ));
                            }
                        }
                    }
                }
            }
        }

        static HRESULT GamepadAdded(
            IInspectable*,
            ABI::Windows::Gaming::Input::IGamepad*
            )
        {
            if (s_game_pad)
            {
                SetEvent(s_game_pad->_changed.in_get());
                if (s_game_pad->_ctrl_changed != INVALID_HANDLE_VALUE)
                {
                    SetEvent(s_game_pad->_ctrl_changed);
                }
            }
            return S_OK;
        }

        static HRESULT GamepadRemoved(
            IInspectable*,
            ABI::Windows::Gaming::Input::IGamepad*
            )
        {
            if (s_game_pad)
            {
                SetEvent(s_game_pad->_changed.in_get());
                if (s_game_pad->_ctrl_changed != INVALID_HANDLE_VALUE)
                {
                    SetEvent(s_game_pad->_ctrl_changed);
                }
            }
            return S_OK;
        }

        static HRESULT UserChanged(
            ABI::Windows::Gaming::Input::IGameController*,
            ABI::Windows::System::IUserChangedEventArgs*
            )
        {
            if (s_game_pad)
            {
                if (s_game_pad->_user_changed != INVALID_HANDLE_VALUE)
                {
                    SetEvent(s_game_pad->_user_changed);
                }
            }
            return S_OK;
        }


    public:
        GamePad* _owner;
        static GamePad::Impl* _s_game_pad;
        HANDLE _ctrl_changed;
        HANDLE _user_changed;

    private:
        int _most_recent_gamepad;
        ComPtr < ABI::Windows::Gaming::Input::IGamepadStatics > _statics;
        ComPtr < ABI::Windows::Gaming::Input::IGamepad > _game_pad[MAX_PLAYER_COUNT];
        EventRegistrationToken _user_change_token[MAX_PLAYER_COUNT];
        EventRegistrationToken _added_token;
        EventRegistrationToken _removed_token;
        std::unique_ptr < void, handle_closer > _changed;
    };
    GamePad::Impl* GamePad::Impl::s_game_pad = nullptr;
#elif defined (_XBOX_ONE)
    // ======================================================================================
    // Windows::Xbox::Input (Xbox One)
    // ======================================================================================
    #include <Windows.Xbox.Input.h>

    #include <Windows.Foundation.Collections.h>
    class GamePad::Impl
    {
    private:
        class GamepadAddedListener : public Microsoft::WRL::RuntimeClass < Microsoft::WRL::RuntimeClassFlags < Microsoft\
            ::WRL::ClassicCom >, ABI::Windows::Foundation::IEventHandler < ABI::Windows::Xbox::Input::\
            GamepadAddedEventArgs* >, Microsoft::WRL::FtmBase >
        {
        public:
            GamepadAddedListener(HANDLE in_event) 
                : _event(in_event) _{} STDMETHOD(Invoke)(
                    _In_ IInspectable*,
                    _In_ ABI::Windows::Xbox::Input::IGamepadAddedEventArgs*
                    ) _override
            {
                SetEvent(_event);
                auto pad = GamePad::Impl::s_game_pad;
                if (pad && pad->_ctrl_changed != INVALID_HANDLE_VALUE)
                {
                    SetEvent(pad->_ctrl_changed);
                }
                return S_OK;
            }


        private:
            HANDLE _event;
        };
        class GamepadRemovedListener : public Microsoft::WRL::RuntimeClass < Microsoft::WRL::RuntimeClassFlags < \
            Microsoft::WRL::ClassicCom >, ABI::Windows::Foundation::IEventHandler < ABI::Windows::Xbox::Input::\
            GamepadRemovedEventArgs* >, Microsoft::WRL::FtmBase >
        {
        public:
            GamepadRemovedListener(HANDLE in_event) 
                : _event(in_event) _{} STDMETHOD(Invoke)(
                    _In_ IInspectable*,
                    _In_ ABI::Windows::Xbox::Input::IGamepadRemovedEventArgs*
                    ) _override
            {
                SetEvent(_event);
                auto pad = GamePad::Impl::s_game_pad;
                if (pad && pad->_ctrl_changed != INVALID_HANDLE_VALUE)
                {
                    SetEvent(pad->_ctrl_changed);
                }
                return S_OK;
            }


        private:
            HANDLE _event;
        };
        class UserPairingListener : public Microsoft::WRL::RuntimeClass < Microsoft::WRL::RuntimeClassFlags < Microsoft\
            ::WRL::ClassicCom >, ABI::Windows::Foundation::IEventHandler < ABI::Windows::Xbox::Input::\
            ControllerPairingChangedEventArgs* >, Microsoft::WRL::FtmBase >
        {
        public:
            UserPairingListener() noexcept _{} STDMETHOD(Invoke)(
                _In_ IInspectable*,
                _In_ ABI::Windows::Xbox::Input::IControllerPairingChangedEventArgs*
                ) _override
            {
                auto pad = GamePad::Impl::s_game_pad;
                if (pad && pad->_user_changed != INVALID_HANDLE_VALUE)
                {
                    SetEvent(pad->_user_changed);
                }
                return S_OK;
            }

        };
        Impl(GamePad* in_owner) 
            : _owner(in_owner)
            , _ctrl_changed(INVALID_HANDLE_VALUE)
            , _user_changed(INVALID_HANDLE_VALUE)
            , _most_recent_gamepad(0)
            , _statics{}
            , _statics_ctrl{}
            , _game_pad{}
            , _added_token{}
            , _removed_token{}
            , _user_paring_token{}
            , _changed{}
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Foundation;
            if (s_game_pad)
            {
                throw std::exception("GamePad is a singleton");
            }
            s_game_pad = this;
            _changed.reset(CreateEventEx(
                nullptr,
                nullptr,
                0,
                EVENT_MODIFY_STATE | SYNCHRONIZE
                ));
            if (!_changed)
            {
                throw std::exception("CreateEventEx");
            }
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Xbox_Input_Gamepad) .Get(),
                _statics.GetAddressOf()
                ));
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Xbox_Input_Controller) .Get(),
                _statics_ctrl.GetAddressOf()
                ));
            ThrowIfFailed(_statics->in_add_gamepad_added(
                Make < GamepadAddedListener > (_changed.get()) .Get(),
                &_added_token
                ));
            ThrowIfFailed(_statics->in_add_gamepad_removed(
                Make < GamepadRemovedListener > (_changed.get()) .Get(),
                &_removed_token
                ));
            ThrowIfFailed(_statics_ctrl->in_add_controller_pairing_changed(
                Make < UserPairingListener > () .Get(),
                &_user_paring_token
                ));
            ScanGamePads();
        }

        ~Impl()
        {
            if (_statics)
            {
                (void)_statics->remove_gamepad_added(_added_token);
                _added_token._value = 0;
                (void)_statics->remove_gamepad_removed(_removed_token);
                _removed_token._value = 0;
                _statics.Reset();
            }
            if (_statics_ctrl)
            {
                (void)_statics_ctrl->remove_controller_pairing_changed(_user_paring_token);
                _user_paring_token._value = 0;
                _statics_ctrl.Reset();
            }
            s_game_pad = nullptr;
        }

        void GetState(
            int in_player,
            _Out_ State&in_state,
            DeadZone in_dead_zone_mode
            )
        {
            using namespace Microsoft::WRL;
            using namespace ABI::Windows::Xbox::Input;
            if (WaitForSingleObjectEx(
                _changed.get(),
                0,
                FALSE
                ) == WAIT_OBJECT_0)
            {
                ScanGamePads();
            }
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if ((in_player >= 0) && (in_player < MAX_PLAYER_COUNT))
            {
                if (_game_pad[in_player])
                {
                    RawGamepadReading reading;
                    HRESULT hr = _game_pad[in_player]->GetRawCurrentReading(&reading);
                    if (SUCCEEDED(hr))
                    {
                        in_state._connected = true;
                        in_state._packet = reading.Timestamp;
                        in_state._buttons._a = (reading.Buttons&GamepadButtons::GamepadButtons_A) != 0;
                        in_state._buttons._b = (reading.Buttons&GamepadButtons::GamepadButtons_B) != 0;
                        in_state._buttons._x = (reading.Buttons&GamepadButtons::GamepadButtons_X) != 0;
                        in_state._buttons._y = (reading.Buttons&GamepadButtons::GamepadButtons_Y) != 0;
                        in_state._buttons._left_stick = (reading.Buttons&GamepadButtons::GamepadButtons_LeftThumbstick) \
                            != 0;
                        in_state._buttons._right_stick = (reading.Buttons&GamepadButtons::GamepadButtons_RightThumbstick\
                            ) != 0;
                        in_state._buttons._left_shoulder = (reading.Buttons&GamepadButtons::GamepadButtons_LeftShoulder)\
                        != 0;
                        in_state._buttons._right_shoulder = (reading.Buttons&GamepadButtons::\
                            GamepadButtons_RightShoulder) != 0;
                        in_state._buttons._back = (reading.Buttons&GamepadButtons::GamepadButtons_View) != 0;
                        in_state._buttons._start = (reading.Buttons&GamepadButtons::GamepadButtons_Menu) != 0;
                        in_state._dpad._up = (reading.Buttons&GamepadButtons::GamepadButtons_DPadUp) != 0;
                        in_state._dpad._down = (reading.Buttons&GamepadButtons::GamepadButtons_DPadDown) != 0;
                        in_state._dpad._right = (reading.Buttons&GamepadButtons::GamepadButtons_DPadRight) != 0;
                        in_state._dpad._left = (reading.Buttons&GamepadButtons::GamepadButtons_DPadLeft) != 0;
                        ApplyStickDeadZone(
                            reading.LeftThumbstickX,
                            reading.LeftThumbstickY,
                            in_dead_zone_mode,
                            1.f,
                            c_xbox_one_thumb_dead_zone,
                            in_state._thumb_sticks._left_x,
                            in_state._thumb_sticks._left_y
                            );
                        ApplyStickDeadZone(
                            reading.RightThumbstickX,
                            reading.RightThumbstickY,
                            in_dead_zone_mode,
                            1.f,
                            c_xbox_one_thumb_dead_zone,
                            in_state._thumb_sticks._right_x,
                            in_state._thumb_sticks._right_y
                            );
                        in_state._triggers._left = reading.LeftTrigger;
                        in_state._triggers._right = reading.RightTrigger;
                        return;
                    }
                }
            }
            memset(
                &in_state,
                0,
                sizeof (State)
                );
        }

        void GetCapabilities(
            int in_player,
            _Out_ Capabilities&in_caps
            )
        {
            using namespace Microsoft::WRL;
            using namespace ABI::Windows::Xbox::Input;
            if (WaitForSingleObjectEx(
                _changed.get(),
                0,
                FALSE
                ) == WAIT_OBJECT_0)
            {
                ScanGamePads();
            }
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if ((in_player >= 0) && (in_player < MAX_PLAYER_COUNT))
            {
                if (_game_pad[in_player])
                {
                    in_caps._connected = true;
                    in_caps._gamepad_type = Capabilities::UNKNOWN;
                    in_caps._id = 0;
                    in_caps._vid = in_caps._pid = 0;
                    ComPtr < IController > ctrl;
                    HRESULT hr = _game_pad[in_player].As(&ctrl);
                    if (SUCCEEDED(hr) && ctrl)
                    {
                        hr = ctrl->get_id(&in_caps._id);
                        if (FAILED(hr)) in_caps._id = 0;
                        Wrappers::HString str;
                        hr = ctrl->get_type(str.GetAddressOf());
                        if (SUCCEEDED(hr))
                        {
                            const wchar_t* type_str = str.GetRawBuffer(nullptr);
                            if (wcsicmp(
                                type_str,
                                L"Windows.Xbox.Input.Gamepad"
                                ) == 0)
                            {
                                in_caps._gamepad_type = Capabilities::GAMEPAD;
                            }
                            else if (wcsicmp(
                                type_str,
                                L"Microsoft.Xbox.Input.ArcadeStick"
                                ) == 0)
                            {
                                in_caps._gamepad_type = Capabilities::ARCADE_STICK;
                            }

                            else if (in_wcsicmp(
                                type_str,
                                L"Microsoft.Xbox.Input.Wheel"
                                ) == 0)
                            {
                                caps._gamepad_type = Capabilities::WHEEL;
                            }

                        }
                    }
#if _XDK_VER >= 0x42ED07E4 /* XDK Edition 180400 */

                        ComPtr < IController3 > ctrl3;
                        hr = _game_pad[player].As(&ctrl3);
                        if (SUCCEEDED(hr) && ctrl3)
                        {
                            if (FAILED(ctrl3->get_hardware_vendor_id(&caps._vid))) caps._vid = 0;
                            if (FAILED(ctrl3->get_hardware_product_id(&caps._pid))) caps._pid = 0;
                        }
#endif

                    return;
                }
            }
            memset(
                &caps,
                0,
                sizeof (Capabilities)
                );
        }

        bool SetVibration(
            int in_player,
            float in_left_motor,
            float in_right_motor,
            float in_left_trigger,
            float in_right_trigger
            ) noexcept
        {
            using namespace ABI::Windows::Xbox::Input;
            if (in_player == c_most_recent) in_player = _most_recent_gamepad;
            if ((in_player >= 0) && (in_player < MAX_PLAYER_COUNT))
            {
                if (_game_pad[in_player])
                {
                    HRESULT hr;
                    try
                    {
                        GamepadVibration vib;
                        vib.LeftMotorLevel = in_left_motor;
                        vib.RightMotorLevel = in_right_motor;
                        vib.LeftTriggerLevel = in_left_trigger;
                        vib.RightTriggerLevel = in_right_trigger;
                        hr = _game_pad[in_player]->SetVibration(vib);
                    }
                    catch (...)
                    {
                        // Handle case where gamepad might be invalid
                        hr = E_FAIL;
                    }

                    if (SUCCEEDED(hr)) return true;
                }
            }
            return false;
        }

        void Suspend() noexcept
        {
            for (size_t j = 0; j < MAX_PLAYER_COUNT;++ j)
            {
                _game_pad[j].Reset();
            }
        }

        void Resume() noexcept
        {
            // Make sure we rescan gamepads
            SetEvent(_changed.in_get());
        }

        void ScanGamePads()
        {
            using namespace ABI::Windows::Foundation::Collections;
            using namespace ABI::Windows::Xbox::Input;
            ComPtr < IVectorView < IGamepad* >> pads;
            ThrowIfFailed(_statics->in_get_gamepads(pads.GetAddressOf()));
            unsigned int count = 0;
            ThrowIfFailed(pads->in_get_size(&count));
            // Check for removed gamepads
            for (size_t j = 0; j < MAX_PLAYER_COUNT;++ j)
            {
                if (_game_pad[j])
                {
                    unsigned int k = 0;
                    for (; k < count;++ k)
                    {
                        ComPtr < IGamepad > pad;
                        HRESULT hr = pads->GetAt(
                            k,
                            pad.GetAddressOf()
                            );
                        if (SUCCEEDED(hr) && (pad == _game_pad[j]))
                        {
                            break;
                        }
                    }
                    if (k >= count)
                    {
                        _game_pad[j].Reset();
                    }
                }
            }
            // Check for added gamepads
            for (unsigned int j = 0; j < count;++ j)
            {
                ComPtr < IGamepad > pad;
                HRESULT hr = pads->GetAt(
                    j,
                    pad.GetAddressOf()
                    );
                if (SUCCEEDED(hr))
                {
                    size_t empty = MAX_PLAYER_COUNT;
                    size_t k = 0;
                    for (; k < MAX_PLAYER_COUNT;++ k)
                    {
                        if (_game_pad[k] == pad)
                        {
                            if (!j) _most_recent_gamepad = static_cast < int > (k);
                            break;
                        }
                        else if (!_game_pad[k])
                        {
                            if (empty >= MAX_PLAYER_COUNT) empty = k;
                        }

                    }
                    if (k >= MAX_PLAYER_COUNT)
                    {
                        if (empty >= MAX_PLAYER_COUNT)
                        {
                            throw std::exception("Too many gamepads found");
                        }
                        _game_pad[empty] = pad;
                        if (!j) _most_recent_gamepad = static_cast < int > (empty);
                    }
                }
            }
        }


    private:
        GamePad* _owner;
        static GamePad::Impl* _s_game_pad;
        HANDLE _ctrl_changed;
        HANDLE _user_changed;
        int _most_recent_gamepad;
        ComPtr < ABI::Windows::Xbox::Input::IGamepadStatics > _statics;
        ComPtr < ABI::Windows::Xbox::Input::IControllerStatics > _statics_ctrl;
        ComPtr < ABI::Windows::Xbox::Input::IGamepad > _game_pad[MAX_PLAYER_COUNT];
        EventRegistrationToken _added_token;
        EventRegistrationToken _removed_token;
        EventRegistrationToken _user_paring_token;
        std::unique_ptr < void, handle_closer > _changed;
    };
    GamePad::Impl* GamePad::Impl::s_game_pad = nullptr;
#else
    // ======================================================================================
    // XInput
    // ======================================================================================
    #include <Xinput.h>


    static_assert (
        GamePad::MAX_PLAYER_COUNT == XUSER_MAX_COUNT,
        "xinput.h mismatch"
        );
    class GamePad::Impl
    {
    public:
        Impl(GamePad* in_owner) 
            : _owner(in_owner)
            , _connected{}
            , _last_read_time{} #if(_WIN32_WINNT < _WIN32_WINNT_WIN8)

                , _left_motor{}
                , _right_motor{}
                , _suspended(false)#endif


        {
            for (int j = 0; j < XUSER_MAX_COUNT;++ j)
            {
                ClearSlot(
                    j,
                    0
                    );
            }
            if (s_game_pad)
            {
                throw std::exception("GamePad is a singleton");
            }
            s_game_pad = this;
        }

        ~Impl()
        {
            s_game_pad = nullptr;
        }

        void GetState(
            int in_player,
            _Out_ State&in_state,
            DeadZone in_dead_zone_mode
            )
        {
            if (in_player == c_most_recent) in_player = GetMostRecent();
            ULONGLONG time = GetTickCount64();
            if (!ThrottleRetry(
                in_player,
                time
                ))
            {
#if(_WIN32_WINNT < _WIN32_WINNT_WIN8)
                    if (_suspended)
                    {
                        memset(
                            &in_state,
                            0,
                            sizeof (State)
                            );
                        in_state._connected = _connected[in_player];
                        return;
                    }
#endif

                XINPUT_STATE xstate;
                DWORD result = XInputGetState(
                    DWORD(in_player),
                    &xstate
                    );
                if (result == ERROR_DEVICE_NOT_CONNECTED)
                {
                    ClearSlot(
                        in_player,
                        time
                        );
                }
                else
                {
                    if (!_connected[in_player]) _last_read_time[in_player] = time;
                    _connected[in_player] = true;
                    in_state._connected = true;
                    in_state._packet = xstate._dw_packet_number;
                    WORD xbuttons = xstate.Gamepad._w_buttons;
                    in_state._buttons._a = (xbuttons&XINPUT_GAMEPAD_A) != 0;
                    in_state._buttons._b = (xbuttons&XINPUT_GAMEPAD_B) != 0;
                    in_state._buttons._x = (xbuttons&XINPUT_GAMEPAD_X) != 0;
                    in_state._buttons._y = (xbuttons&XINPUT_GAMEPAD_Y) != 0;
                    in_state._buttons._left_stick = (xbuttons&XINPUT_GAMEPAD_LEFT_THUMB) != 0;
                    in_state._buttons._right_stick = (xbuttons&XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
                    in_state._buttons._left_shoulder = (xbuttons&XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
                    in_state._buttons._right_shoulder = (xbuttons&XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
                    in_state._buttons._back = (xbuttons&XINPUT_GAMEPAD_BACK) != 0;
                    in_state._buttons._start = (xbuttons&XINPUT_GAMEPAD_START) != 0;
                    in_state._dpad._up = (xbuttons&XINPUT_GAMEPAD_DPAD_UP) != 0;
                    in_state._dpad._down = (xbuttons&XINPUT_GAMEPAD_DPAD_DOWN) != 0;
                    in_state._dpad._right = (xbuttons&XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
                    in_state._dpad._left = (xbuttons&XINPUT_GAMEPAD_DPAD_LEFT) != 0;
                    if (in_dead_zone_mode == DEAD_ZONE_NONE)
                    {
                        in_state._triggers._left = ApplyLinearDeadZone(
                            float (xstate.Gamepad._left_trigger),
                            255.f,
                            0.f
                            );
                        in_state._triggers._right = ApplyLinearDeadZone(
                            float (xstate.Gamepad._right_trigger),
                            255.f,
                            0.f
                            );
                    }
                    else
                    {
                        in_state._triggers._left = ApplyLinearDeadZone(
                            float (xstate.Gamepad._left_trigger),
                            255.f,
                            float (XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                            );
                        in_state._triggers._right = ApplyLinearDeadZone(
                            float (xstate.Gamepad._right_trigger),
                            255.f,
                            float (XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                            );
                    }
                    ApplyStickDeadZone(
                        float (xstate.Gamepad._s_thumb_lx),
                        float (xstate.Gamepad._s_thumb_ly),
                        in_dead_zone_mode,
                        32767.f,
                        float (XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
                        in_state._thumb_sticks._left_x,
                        in_state._thumb_sticks._left_y
                        );
                    ApplyStickDeadZone(
                        float (xstate.Gamepad._s_thumb_rx),
                        float (xstate.Gamepad._s_thumb_ry),
                        in_dead_zone_mode,
                        32767.f,
                        float (XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
                        in_state._thumb_sticks._right_x,
                        in_state._thumb_sticks._right_y
                        );
                    return;
                }
            }
            memset(
                &in_state,
                0,
                sizeof (State)
                );
        }

        void GetCapabilities(
            int in_player,
            _Out_ Capabilities&in_caps
            )
        {
            if (in_player == c_most_recent) in_player = GetMostRecent();
            ULONGLONG time = GetTickCount64();
            if (!ThrottleRetry(
                in_player,
                time
                ))
            {
                XINPUT_CAPABILITIES xcaps;
                DWORD result = XInputGetCapabilities(
                    DWORD(in_player),
                    0,
                    &xcaps
                    );
                if (result == ERROR_DEVICE_NOT_CONNECTED)
                {
                    ClearSlot(
                        in_player,
                        time
                        );
                }
                else
                {
                    if (!_connected[in_player]) _last_read_time[in_player] = time;
                    _connected[in_player] = true;
                    in_caps._connected = true;
                    in_caps._id = uint64_t (in_player);
                    if (xcaps.Type == XINPUT_DEVTYPE_GAMEPAD)
                    {
                        static_assert (
                            Capabilities::GAMEPAD == XINPUT_DEVSUBTYPE_GAMEPAD,
                            "xinput.h mismatch"
                            );
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN8)
                            static_assert (
                                XINPUT_DEVSUBTYPE_WHEEL == Capabilities::WHEEL,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_ARCADE_STICK == Capabilities::ARCADE_STICK,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_FLIGHT_STICK == Capabilities::FLIGHT_STICK,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_DANCE_PAD == Capabilities::DANCE_PAD,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_GUITAR == Capabilities::GUITAR,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE == Capabilities::GUITAR_ALTERNATE,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_DRUM_KIT == Capabilities::DRUM_KIT,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_GUITAR_BASS == Capabilities::GUITAR_BASS,
                                "xinput.h mismatch"
                                );
                            static_assert (
                                XINPUT_DEVSUBTYPE_ARCADE_PAD == Capabilities::ARCADE_PAD,
                                "xinput.h mismatch"
                                );
#endif

                        in_caps._gamepad_type = Capabilities::Type(xcaps.SubType);
                    }
                    // Hard-coded VID/PID
                    in_caps._vid = 0x045E;
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN8)
                        in_caps._pid = (xcaps.Flags&XINPUT_CAPS_WIRELESS) ? 0x0719 : 0;
#else
                        in_caps._pid = 0;
#endif

                    return;
                }
            }
            memset(
                &in_caps,
                0,
                sizeof (Capabilities)
                );
        }

        bool SetVibration(
            int in_player,
            float in_left_motor,
            float in_right_motor,
            float in_left_trigger,
            float in_right_trigger
            ) noexcept
        {
            if (in_player == c_most_recent) in_player = GetMostRecent();
            ULONGLONG time = GetTickCount64();
            if (ThrottleRetry(
                in_player,
                time
                ))
            {
                return false;
            }
            // XInput does not provide a way to set the left/right trigger impulse motors on the Xbox One Controller,
            // And these motors are not present on the Xbox 360 Common Controller
            UNREFERENCED_PARAMETER(in_left_trigger);
            UNREFERENCED_PARAMETER(in_right_trigger);
#if(_WIN32_WINNT < _WIN32_WINNT_WIN8)
                _left_motor[in_player] = in_left_motor;
                _right_motor[in_player] = in_right_motor;
                if (_suspended) return _connected[in_player];
#endif

            XINPUT_VIBRATION xvibration;
            xvibration._w_left_motor_speed = WORD(in_left_motor* 0xFFFF);
            xvibration._w_right_motor_speed = WORD(in_right_motor* 0xFFFF);
            DWORD result = XInputSetState(
                DWORD(in_player),
                &xvibration
                );
            if (result == ERROR_DEVICE_NOT_CONNECTED)
            {
                ClearSlot(
                    in_player,
                    time
                    );
                return false;
            }
            else
            {
                if (!_connected[in_player]) _last_read_time[in_player] = time;
                _connected[in_player] = true;
                return (result == ERROR_SUCCESS);
            }
        }

        void Suspend() noexcept
        {
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN10)
                // XInput focus is handled automatically on Windows 10
#elif(_WIN32_WINNT >= _WIN32_WINNT_WIN8)
                XInputEnable(FALSE);
#else
                // For XInput 9.1.0, we have to emulate the behavior of XInputEnable( FALSE )
                if (!_suspended)
                {
                    for (size_t j = 0; j < XUSER_MAX_COUNT;++ j)
                    {
                        if (_connected[j])
                        {
                            XINPUT_VIBRATION xvibration;
                            xvibration._w_left_motor_speed = xvibration._w_right_motor_speed = 0;
                            (void)XInputSetState(
                                DWORD(j),
                                &xvibration
                                );
                        }
                    }
                    _suspended = true;
                }
#endif

        }

        void Resume() noexcept
        {
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN10)
                // XInput focus is handled automatically on Windows 10
#elif(_WIN32_WINNT >= _WIN32_WINNT_WIN8)
                XInputEnable(TRUE);
#else
                // For XInput 9.1.0, we have to emulate the behavior of XInputEnable( TRUE )
                if (_suspended)
                {
                    ULONGLONG time = GetTickCount64();
                    for (int j = 0; j < XUSER_MAX_COUNT;++ j)
                    {
                        if (_connected[j])
                        {
                            XINPUT_VIBRATION xvibration;
                            xvibration._w_left_motor_speed = WORD(_left_motor[j]* 0xFFFF);
                            xvibration._w_right_motor_speed = WORD(_right_motor[j]* 0xFFFF);
                            DWORD result = XInputSetState(
                                DWORD(j),
                                &xvibration
                                );
                            if (result == ERROR_DEVICE_NOT_CONNECTED)
                            {
                                ClearSlot(
                                    j,
                                    time
                                    );
                            }
                        }
                    }
                    _suspended = false;
                }
#endif

        }


    private:
        bool ThrottleRetry(
            int in_player,
            ULONGLONG in_time
            )
        {
            // This function minimizes a potential performance issue with XInput on Windows when
            // Checking a disconnected controller slot which requires device enumeration.
            // This throttling keeps checks for newly connected gamepads to about once a second
            if ((in_player < 0) || (in_player >= XUSER_MAX_COUNT)) return true;
            if (_connected[in_player]) return false;
            for (int j = 0; j < XUSER_MAX_COUNT;++ j)
            {
                if (!_connected[j])
                {
                    LONGLONG delta = LONGLONG(in_time) - LONGLONG(_last_read_time[j]);
                    LONGLONG interval = 1000;
                    if (j != in_player) interval / = 4;
                    if ((delta >= 0) && (delta < interval)) return true;
                }
            }
            return false;
        }

        void ClearSlot(
            int in_player,
            ULONGLONG in_time
            )
        {
            _connected[in_player] = false;
            _last_read_time[in_player] = in_time;
#if(_WIN32_WINNT < _WIN32_WINNT_WIN8)
                _left_motor[in_player] = _right_motor[in_player] = 0.f;
#endif

        }

        int GetMostRecent()
        {
            int player = - 1;
            ULONGLONG time = 0;
            for (size_t j = 0; j < XUSER_MAX_COUNT;++ j)
            {
                if (_connected[j] && (_last_read_time[j] > time))
                {
                    time = _last_read_time[j];
                    player = static_cast < int > (j);
                }
            }
            return player;
        }


    public:
        GamePad* _owner;
        static GamePad::Impl* _s_game_pad;

    private:
        bool _connected[XUSER_MAX_COUNT];
        ULONGLONG _last_read_time[XUSER_MAX_COUNT];
#if(_WIN32_WINNT < _WIN32_WINNT_WIN8)
            // Variables for emulating XInputEnable on XInput 9.1.0
            float _left_motor[XUSER_MAX_COUNT];
            float _right_motor[XUSER_MAX_COUNT];
            bool _suspended;
#endif

    };
    GamePad::Impl* GamePad::Impl::s_game_pad = nullptr;
#endif

#pragma warning(disable : 4355)
// Public constructor.
GamePad::GamePad() noexcept (false) 
    : impl(std::make_unique < Impl > (this)){}

// Move constructor.
GamePad::GamePad(GamePad && in_move_from) noexcept 
    : impl(std::move(in_move_from.impl))
{
    impl->_owner = this;
}

// Move assignment.
GamePad&GamePad::operator=(GamePad && in_move_from) noexcept
{
    impl = std::move(in_move_from.impl);
    impl->_owner = this;
    return* this;
}

// Public destructor.
GamePad::~GamePad(){}

GamePad::State GamePad::GetState(
    int in_player,
    DeadZone in_dead_zone_mode
    )
{
    State state;
    impl->GetState(
        in_player,
        state,
        in_dead_zone_mode
        );
    return state;
}

GamePad::Capabilities GamePad::GetCapabilities(int in_player)
{
    Capabilities caps;
    impl->GetCapabilities(
        in_player,
        caps
        );
    return caps;
}

bool GamePad::SetVibration(
    int in_player,
    float in_left_motor,
    float in_right_motor,
    float in_left_trigger,
    float in_right_trigger
    ) noexcept
{
    return impl->SetVibration(
        in_player,
        in_left_motor,
        in_right_motor,
        in_left_trigger,
        in_right_trigger
        );
}

void GamePad::Suspend() noexcept
{
    impl->Suspend();
}

void GamePad::Resume() noexcept
{
    impl->Resume();
}

#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
    void GamePad::RegisterEvents(HANDLE in_ctrl_changed) noexcept
    {
        impl->_ctrl_changed = (!in_ctrl_changed) ? INVALID_HANDLE_VALUE : in_ctrl_changed;
    }

    void GamePad::GetDevice(
        int in_player,
        in_outptr_ IGameInputDevice** in_device
        ) noexcept
    {
        impl->GetDevice(
            in_player,
            in_device
            );
    }

#elif((_WIN32_WINNT >= _WIN32_WINNT_WIN10) && !defined (_GAMING_DESKTOP)) || defined (_XBOX_ONE)
    void GamePad::RegisterEvents(
        HANDLE in_ctrl_changed,
        HANDLE in_user_changed
        ) noexcept
    {
        impl->_ctrl_changed = (!in_ctrl_changed) ? INVALID_HANDLE_VALUE : in_ctrl_changed;
        impl->_user_changed = (!in_user_changed) ? INVALID_HANDLE_VALUE : in_user_changed;
    }

#endif

GamePad&GamePad::Get()
{
    if (!Impl::s_game_pad || !Impl::s_game_pad->_owner) 
        throw std::exception("GamePad is a singleton");
    return* Impl::s_game_pad->_owner;
}

// ======================================================================================
// ButtonStateTracker
// ======================================================================================
#define UPDATE_BUTTON_STATE(in_field) in_field = static_cast < ButtonState > ((!!state._buttons._field) | ((!!state.buttons.\
    in_field ^ !!lastState._buttons._field) << 1));
void GamePadButtonStateTracker::Update(const GamePad::State&in_state) noexcept
{
    UPDATE_BUTTON_STATE(_a) 

    assert((!in_state._buttons._a && !last_state._buttons._a) == (_a == UP));
    assert((in_state._buttons._a && last_state._buttons._a) == (_a == HELD));
    assert((!in_state._buttons._a && last_state._buttons._a) == (_a == RELEASED));
    assert((in_state._buttons._a && !last_state._buttons._a) == (_a == PRESSED));

    UPDATE_BUTTON_STATE(_b) 
    UPDATE_BUTTON_STATE(_x) 
    UPDATE_BUTTON_STATE(_y) 
    UPDATE_BUTTON_STATE(_left_stick) 
    UPDATE_BUTTON_STATE(_right_stick)
    UPDATE_BUTTON_STATE(_left_shoulder)
    UPDATE_BUTTON_STATE(_right_shoulder)
    UPDATE_BUTTON_STATE(_back)
    UPDATE_BUTTON_STATE(_start)
    
    _dpad_up = static_cast < ButtonState > ((!!in_state._dpad.\
        _up) | ((!!in_state._dpad._up ^ !!last_state._dpad._up) << 1));
    _dpad_down = static_cast < ButtonState > ((!!in_state._dpad._down) | ((!!in_state._dpad._down ^ !!last_state._dpad.\
        _down) << 1));
    _dpad_left = static_cast < ButtonState > ((!!in_state._dpad._left) | ((!!in_state._dpad._left ^ !!last_state._dpad.\
        _left) << 1));
    _dpad_right = static_cast < ButtonState > ((!!in_state._dpad._right) | ((!!in_state._dpad._right ^ !!last_state._dpad\
        ._right) << 1));

    assert((!in_state._dpad._up && !last_state._dpad._up) == (dpad_up == UP));
    assert((in_state._dpad._up && last_state._dpad._up) == (dpad_up == HELD));
    assert((!in_state._dpad._up && last_state._dpad._up) == (dpad_up == RELEASED));
    assert((in_state._dpad._up && !last_state._dpad._up) == (dpad_up == PRESSED));

    // Handle 'threshold' tests which emulate buttons
    bool threshold = in_state.IsLeftThumbStickUp();
    _left_stick_up = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsLeftThumbStickUp()) << 1\
        ));
    threshold = in_state.IsLeftThumbStickDown();
    _left_stick_down = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsLeftThumbStickDown()) \
        << 1));
    threshold = in_state.IsLeftThumbStickLeft();
    _left_stick_left = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsLeftThumbStickLeft()) \
        << 1));
    threshold = in_state.IsLeftThumbStickRight();
    _left_stick_right = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsLeftThumbStickRight()\
        ) << 1));
    threshold = in_state.IsRightThumbStickUp();
    _right_stick_up = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsRightThumbStickUp()) <<\
    1));
    threshold = in_state.IsRightThumbStickDown();
    _right_stick_down = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsRightThumbStickDown()\
        ) << 1));
    threshold = in_state.IsRightThumbStickLeft();
    _right_stick_left = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsRightThumbStickLeft()\
        ) << 1));
    threshold = in_state.IsRightThumbStickRight();
    _right_stick_right = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsRightThumbStickRight\
        ()) << 1));
    threshold = in_state.IsLeftTriggerPressed();
    _left_trigger = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsLeftTriggerPressed()) << \
        1));
    threshold = in_state.IsRightTriggerPressed();
    _right_trigger = static_cast < ButtonState > ((!!threshold) | ((!!threshold ^ !!last_state.IsRightTriggerPressed()) \
        << 1));
    last_state = in_state;
}

#undef UPDATE_BUTTON_STATE
void GamePadButtonStateTracker::Reset() noexcept
{
    memset(
        this,
        0,
        sizeof (GamePadButtonStateTracker)
        );
}

