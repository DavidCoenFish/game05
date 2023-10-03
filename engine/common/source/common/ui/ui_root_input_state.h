#pragma once

#include "common/math/vector_float2.h"

class VectorInt2;
struct UIManagerDealInputParam;

class UIRootInputState
{
public:
    UIRootInputState();

    void Update(
        const UIManagerDealInputParam& in_param,
        const VectorInt2& in_root_size
        );

    const VectorFloat2& GetMousePosRef() const { return _mouse_pos; }
    const bool GetMouseLeftDown() const { return _mouse_prev_left_down; }
    const bool GetMouseLeftDownChange() const { return _mouse_left_down_change; }

private:
    // current focus, using unique pointer so no weak pointer, so just travers hierarchy to update flag?
    void* _focus_node;

    // node that click started on
    // node that drag started from
    // node being dragged

    bool _mouse_valid;
    int _mouse_prev_x;
    int _mouse_prev_y;
    int _mouse_delta_x;
    int _mouse_delta_y;
    bool _mouse_prev_left_down;
    bool _mouse_left_down_change;

    VectorFloat2 _mouse_pos;

};
