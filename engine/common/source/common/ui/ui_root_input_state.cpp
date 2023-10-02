#include "common/common_pch.h"
#include "common/ui/ui_root_input_state.h"
#include "common/ui/ui_manager.h"

UIRootInputState::UIRootInputState()
    : _focus_node(nullptr)
    , _mouse_valid(false)
    , _mouse_prev_x(0)
    , _mouse_prev_y(0)
    , _mouse_delta_x(0)
    , _mouse_delta_y(0)
{
    // Nop
}

void UIRootInputState::Update(const UIManagerDealInputParam& in_param)
{
    if (_mouse_valid && in_param._mouse_valid)
    {
        _mouse_delta_x = in_param._mouse_x - _mouse_prev_x;
        _mouse_delta_y = in_param._mouse_y - _mouse_prev_y;
    }
    else
    {
        _mouse_delta_x = 0;
        _mouse_delta_y = 0;
    }
    _mouse_valid = in_param._mouse_valid;
    _mouse_prev_x = in_param._mouse_x;
    _mouse_prev_y = in_param._mouse_y;

    return;
}

