// --------------------------------------------------------------------------------------
// File: Keyboard.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkId=248929
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#include "common/common_pch.h"

#include "common/direct_xtk12/keyboard.h"
using namespace DirectX;
using Microsoft::WRL::ComPtr;
static_assert (
    sizeof (Keyboard::State) == (256 / 8),
    "Size mismatch for State"
    );
#ifdef __clang__
    #pragma clang diagnostic ignored "-Wunused-function"
#endif

namespace
{
    inline void KeyDown(
        int in_key,
        Keyboard::State&in_state
        ) noexcept
    {
        if (in_key < 0 || in_key > 0xfe) return;
        auto ptr = reinterpret_cast < uint32_t* > (&in_state);
        unsigned int bf = 1u << (in_key&0x1f);
        ptr[(in_key >> 5)] |= bf;
    }

    inline void KeyUp(
        int in_key,
        Keyboard::State&in_state
        ) noexcept
    {
        if (in_key < 0 || in_key > 0xfe) return;
        auto ptr = reinterpret_cast < uint32_t* > (&in_state);
        unsigned int bf = 1u << (in_key&0x1f);
        ptr[(in_key >> 5)] &= ~bf;
    }

}
#if defined (WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
    #include <GameInput.h>

    // ======================================================================================
    // GameInput
    // ======================================================================================
    class Keyboard::Impl
    {
    public:
        Impl(Keyboard* in_owner) 
            : _owner(in_owner)
            , _connected(0)
            , _device_token(0)
            , _key_state{}
        {
            if (s_keyboard)
            {
                throw std::exception("Keyboard is a singleton");
            }
            s_keyboard = this;
            ThrowIfFailed(GameInputCreate(_game_input.GetAddressOf()));
            ThrowIfFailed(_game_input->RegisterDeviceCallback(
                nullptr,
                GameInputKindKeyboard,
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
                            "ERROR: GameInput::UnregisterCallback [keyboard] failed (%08X)",
                            static_cast < unsigned int > (hr)
                            );
                    }
                }
                _device_token = 0;
            }
            s_keyboard = nullptr;
        }

        void GetState(State&in_state) const
        {
            in_state = {};
            ComPtr < IGameInputReading > reading;
            if (SUCCEEDED(_game_input->GetCurrentReading(
                GameInputKindKeyboard,
                nullptr,
                reading.GetAddressOf()
                )))
            {
                uint32_t read_count = reading->GetKeyState(
                    c_max_simultaneous_keys,
                    _key_state
                    );
                for (size_t j = 0; j < read_count;++ j)
                {
                    int vk = static_cast < int > (_key_state[j]._virtual_key);
                    KeyDown(
                        vk,
                        in_state
                        );
                }
            }
        }

        void Reset() noexcept{}

        bool IsConnected() const
        {
            return _connected > 0;
        }


    private:
        static void CALLBACK OnGameInputDevice(
            _In_ GameInputCallbackToken,
            _In_ void* in_context,
            _In_ IGameInputDevice*,
            _In_ uint64_t,
            _In_ GameInputDeviceStatus in_current_status,
            _In_ GameInputDeviceStatus
            ) noexcept
        {
            auto impl = reinterpret_cast < Keyboard::Impl* > (in_context);
            if (in_current_status&GameInputDeviceConnected)
            {
                ++ impl->_connected;
            }
            else if (impl->_connected > 0)
            {
                -- impl->_connected;
            }

        }


    public:
        Keyboard* _owner;
        uint32_t _connected;
        static Keyboard::Impl* _s_keyboard;

    private:
        static constexpr size_t c_max_simultaneous_keys = 16;
        ComPtr < IGameInput > _game_input;
        GameInputCallbackToken _device_token;
        mutable GameInputKeyState _key_state[c_max_simultaneous_keys];
    };
    Keyboard::Impl* Keyboard::Impl::s_keyboard = nullptr;
#elif !defined (WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
    // ======================================================================================
    // Win32 desktop implementation
    // ======================================================================================
    //
    // For a Win32 desktop application, call this function from your Window Message Procedure
    //
    /*
    LRESULT CALLBACK WndProc(HWND in_hwnd, UINT in_message, WPARAM in_w_param, LPARAM in_l_param)
    {
        switch (in_message)
        {
        default:
            break;    
        case WM_ACTIVATEAPP:
            Keyboard::ProcessMessage(in_message, in_w_param, in_l_param);
            break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            Keyboard::ProcessMessage(in_message, in_w_param, in_l_param);
            break;

        }
    }
    */
    
    class Keyboard::Impl
    {
    public:
        Impl(Keyboard* in_owner) 
            : _state{}
            , _owner(in_owner)
        {
            if (s_keyboard)
            {
                throw std::exception("Keyboard is a singleton");
            }
            s_keyboard = this;
        }

        Impl(Impl &&) = default;
        Impl& operator=(Impl &&) = default;
        Impl(Impl const&) = delete;
        Impl& operator=(Impl const&) = delete;
        ~Impl()
        {
            s_keyboard = nullptr;
        }

        void GetState(State&in_state) const
        {
            memcpy(
                &in_state,
                &_state,
                sizeof (State)
                );
        }

        void Reset() noexcept
        {
            memset(
                &_state,
                0,
                sizeof (State)
                );
        }

        bool IsConnected() const
        {
            return true;
        }


    public:
        State _state;
        Keyboard* _owner;
        static Keyboard::Impl* s_keyboard;
    };
    Keyboard::Impl* Keyboard::Impl::s_keyboard = nullptr;
    void Keyboard::ProcessMessage(
        UINT in_message,
        WPARAM in_w_param,
        LPARAM in_l_param
        )
    {
        auto impl = Impl::s_keyboard;
        if (!impl) 
            return;
        bool down = false;
        switch (in_message)
        {
        case WM_ACTIVATEAPP:
            impl->Reset();
            return;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            down = true;
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            break;
        default:
            return;
        }

        int vk = static_cast < int > (in_w_param);
        switch (vk)
        {
        case VK_SHIFT:
            vk = static_cast < int > (MapVirtualKey(
                (static_cast < UINT > (in_l_param) &0x00ff0000) >> 16u,
                MAPVK_VSC_TO_VK_EX
                ));
            if (!down)
            {
                // Workaround to ensure left vs. right shift get cleared when both were pressed at same time
                KeyUp(
                    VK_LSHIFT,
                    impl->_state
                    );
                KeyUp(
                    VK_RSHIFT,
                    impl->_state
                    );
            }
            break;
        case VK_CONTROL:
            vk = (static_cast < UINT > (in_l_param) &0x01000000) ? VK_RCONTROL : VK_LCONTROL;
            break;
        case VK_MENU:
            vk = (static_cast < UINT > (in_l_param) &0x01000000) ? VK_RMENU : VK_LMENU;
            break;
        }

        if (down)
        {
            KeyDown(
                vk,
                impl->_state
                );
        }
        else
        {
            KeyUp(
                vk,
                impl->_state
                );
        }
    }

#else
    // ======================================================================================
    // Windows Store or Universal Windows Platform (UWP) app implementation
    // ======================================================================================
    //
    // For a Windows Store app or Universal Windows Platform (UWP) app, add the following:
    //
    // Void App::SetWindow(CoreWindow^ window )
    // {
    // M_keyboard->SetWindow(window);
    // }
    //
    #include <Windows.Devices.Input.h>


    class Keyboard::Impl
    {
    public:
        Impl(Keyboard* in_owner) 
            : _state{}
            , _owner(in_owner)
            , _accelerator_key_token{}
            , _activated_token{}
        {
            if (s_keyboard)
            {
                throw std::exception("Keyboard is a singleton");
            }
            s_keyboard = this;
        }

        ~Impl()
        {
            s_keyboard = nullptr;
            RemoveHandlers();
        }

        void GetState(State&in_state) const
        {
            memcpy(
                &in_state,
                &_state,
                sizeof (State)
                );
        }

        void Reset() noexcept
        {
            memset(
                &_state,
                0,
                sizeof (State)
                );
        }

        bool IsConnected() const
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::Devices::Input;
            using namespace ABI::Windows::Foundation;
            ComPtr < IKeyboardCapabilities > caps;
            HRESULT hr = RoActivateInstance(
                HStringReference(RuntimeClass_Windows_Devices_Input_KeyboardCapabilities) .Get(),
                &caps
                );
            ThrowIfFailed(hr);
            INT32 value;
            if (SUCCEEDED(caps->get_keyboard_present(&value)))
            {
                return value != 0;
            }
            return false;
        }

        void SetWindow(ABI::Windows::UI::Core::ICoreWindow* in_window)
        {
            using namespace Microsoft::WRL;
            using namespace Microsoft::WRL::Wrappers;
            using namespace ABI::Windows::UI::Core;
            if (_window.Get() == in_window) return;
            RemoveHandlers();
            _window = in_window;
            if (!in_window) return;
            typedef \
                _fi_typed_event_handler_2_windows_cu_i_c_core_c_core_window_windows_cu_i_c_core_c_window_activated_event_args\
            ActivatedHandler;
            HRESULT hr = in_window->add_activated(
                Callback < ActivatedHandler > (Activated) .Get(),
                &_activated_token
                );
            ThrowIfFailed(hr);
            ComPtr < ICoreDispatcher > dispatcher;
            hr = in_window->get_dispatcher(dispatcher.GetAddressOf());
            ThrowIfFailed(hr);
            ComPtr < ICoreAcceleratorKeys > keys;
            hr = dispatcher.As(&keys);
            ThrowIfFailed(hr);
            typedef \
                _fi_typed_event_handler_2_windows_cu_i_c_core_c_core_dispatcher_windows_cu_i_c_core_c_accelerator_key_event_args\
            AcceleratorKeyHandler;
            hr = keys->add_accelerator_key_activated(
                Callback < AcceleratorKeyHandler > (AcceleratorKeyEvent) .Get(),
                &_accelerator_key_token
                );
            ThrowIfFailed(hr);
        }


    private:
        void RemoveHandlers()
        {
            if (_window)
            {
                using namespace ABI::Windows::UI::Core;
                ComPtr < ICoreDispatcher > dispatcher;
                HRESULT hr = _window->get_dispatcher(dispatcher.GetAddressOf());
                ThrowIfFailed(hr);
                (void)_window->remove_activated(_activated_token);
                _activated_token._value = 0;
                ComPtr < ICoreAcceleratorKeys > keys;
                hr = dispatcher.As(&keys);
                ThrowIfFailed(hr);
                (void)keys->remove_accelerator_key_activated(_accelerator_key_token);
                _accelerator_key_token._value = 0;
            }
        }

        static HRESULT Activated(
            IInspectable*,
            ABI::Windows::UI::Core::IWindowActivatedEventArgs*
            )
        {
            auto impl = Impl::s_keyboard;
            if (!impl) return S_OK;
            impl->Reset();
            return S_OK;
        }

        static HRESULT AcceleratorKeyEvent(
            IInspectable*,
            ABI::Windows::UI::Core::IAcceleratorKeyEventArgs* in_args
            )
        {
            using namespace ABI::Windows::System;
            using namespace ABI::Windows::UI::Core;
            auto impl = Impl::s_keyboard;
            if (!impl) return S_OK;
            CoreAcceleratorKeyEventType evt_type;
            HRESULT hr = in_args->get_event_type(&evt_type);
            ThrowIfFailed(hr);
            bool down = false;
            switch (evt_type)
            {
            case CoreAcceleratorKeyEventType_KeyDown:
            case CoreAcceleratorKeyEventType_SystemKeyDown:
                down = true;
                break;
            case CoreAcceleratorKeyEventType_KeyUp:
            case CoreAcceleratorKeyEventType_SystemKeyUp:
                break;
            default:
                return S_OK;
            }

            CorePhysicalKeyStatus status;
            hr = args->get_key_status(&status);
            ThrowIfFailed(hr);
            VirtualKey virtual_key;
            hr = args->get_virtual_key(&virtual_key);
            ThrowIfFailed(hr);
            int vk = static_cast < int > (virtual_key);
            switch (vk)
            {
            case VK_SHIFT:
                vk = (status.ScanCode == 0x36) ? VK_RSHIFT : VK_LSHIFT;
                if (!down)
                {
                    // Workaround to ensure left vs. right shift get cleared when both were pressed at same time
                    KeyUp(
                        VK_LSHIFT,
                        in_impl->_state
                        );
                    KeyUp(
                        VK_RSHIFT,
                        in_impl->_state
                        );
                }
                break;
            case VK_CONTROL:
                vk = (status.IsExtendedKey) ? VK_RCONTROL : VK_LCONTROL;
                break;
            case VK_MENU:
                vk = (status.IsExtendedKey) ? VK_RMENU : VK_LMENU;
                break;
            }

            if (down)
            {
                KeyDown(
                    in_vk,
                    in_impl->_state
                    );
            }
            else
            {
                KeyUp(
                    in_vk,
                    in_impl->_state
                    );
            }
            return S_OK;
        }


    public:
        State _state;
        Keyboard* _owner;
        static Keyboard::Impl* _s_keyboard;

    private:
        ComPtr < ABI::Windows::UI::Core::ICoreWindow > _window;
        EventRegistrationToken _accelerator_key_token;
        EventRegistrationToken _activated_token;
    };
    Keyboard::Impl* Keyboard::Impl::s_keyboard = nullptr;
    void Keyboard::SetWindow(ABI::Windows::UI::Core::ICoreWindow* in_window)
    {
        impl->SetWindow(in_window);
    }

#endif

#pragma warning(disable : 4355)
// Public constructor.
Keyboard::Keyboard() noexcept (false) 
    : _impl(std::make_unique < Impl > (this)){}

// Move constructor.
Keyboard::Keyboard(Keyboard && in_move_from) noexcept 
    : _impl(std::move(in_move_from._impl))
{
    _impl->_owner = this;
}

// Move assignment.
Keyboard&Keyboard::operator=(Keyboard && in_move_from) noexcept
{
    _impl = std::move(in_move_from._impl);
    _impl->_owner = this;
    return* this;
}

// Public destructor.
Keyboard::~Keyboard(){}

Keyboard::State Keyboard::GetState() const
{
    State state;
    _impl->GetState(state);
    return state;
}

void Keyboard::Reset() noexcept
{
    _impl->Reset();
}

bool Keyboard::IsConnected() const
{
    return _impl->IsConnected();
}

Keyboard& Keyboard::Get()
{
    if (!Impl::s_keyboard || !Impl::s_keyboard->_owner) 
    {
        throw std::exception("Keyboard is a singleton");
    }
    return* Impl::s_keyboard->_owner;
}

// ======================================================================================
// KeyboardStateTracker
// ======================================================================================
void KeyboardButtonStateTracker::Update(const Keyboard::State& in_state) noexcept
{
    auto curr_ptr = reinterpret_cast < const uint32_t* > (&in_state);
    auto prev_ptr = reinterpret_cast < const uint32_t* > (&_last_state);
    auto released_ptr = reinterpret_cast < uint32_t* > (&_released);
    auto pressed_ptr = reinterpret_cast < uint32_t* > (&_pressed);
    for (size_t j = 0; j < (256 / 32);++ j)
    {
        * pressed_ptr =* curr_ptr&~ (* prev_ptr);
        * released_ptr = ~ (* curr_ptr) &* prev_ptr;
        ++ curr_ptr;
        ++ prev_ptr;
        ++ released_ptr;
        ++ pressed_ptr;
    }
    _last_state = in_state;
}

void KeyboardButtonStateTracker::Reset() noexcept
{
    memset(
        this,
        0,
        sizeof (KeyboardButtonStateTracker)
        );
}

