#include "common/common_pch.h"
#include "common/ui/ui_root_input_state.h"
#include "common/ui/ui_manager.h"
#include "common/log/log.h"
#include "common/math/dsc_math.h"

UIRootInputState::UIRootInputState()
    : _focus_node(nullptr)
    , _mouse_valid(false)
    , _mouse_prev_x(0)
    , _mouse_prev_y(0)
    , _mouse_delta_x(0)
    , _mouse_delta_y(0)
    , _mouse_prev_left_down(false)
    , _mouse_left_down_change(false)
{
    // Nop
}

void UIRootInputState::Update(
    const UIManagerDealInputParam& in_param,
    const VectorInt2& in_root_size
    )
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
    _mouse_left_down_change = _mouse_prev_left_down != in_param._mouse_left_down;
    _mouse_prev_left_down = in_param._mouse_left_down;

    _mouse_pos = VectorFloat2(
        DscMath::ConvertZeroOneToNegativeOneOne(in_root_size[0] ? static_cast<float>(in_param._mouse_x) / static_cast<float>(in_root_size[0]) : 0.0f),
        DscMath::ConvertZeroOneToNegativeOneOne(in_root_size[1] ? 1.0f - (static_cast<float>(in_param._mouse_y) / static_cast<float>(in_root_size[1])) : 0.0f)
        );

    //LOG_MESSAGE_DEBUG("%f %f %d %d", _mouse_pos[0], _mouse_pos[1], _mouse_prev_left_down, _mouse_left_down_change);

    return;
}

