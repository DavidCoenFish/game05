#pragma once

#include "common/math/vector_float2.h"

class VectorInt2;
struct UIManagerDealInputParam;

struct UIRootInputStateTouch
{
    explicit UIRootInputStateTouch(
        const VectorFloat2& in_touch_pos_current = VectorFloat2(),
        //const VectorFloat2& in_touch_pos_active_start = VectorFloat2(),
        void* const in_active_source_token = nullptr,
        const int in_id = 0,
        const float in_active_duration = 0.0f,
        const bool in_active = false,
        //const bool in_start = false,
        const bool in_end = false
        );

    /// where is the touch currently, in screen space lower left [-1,-1 ... 1,1]
    VectorFloat2 _touch_pos_current;
    /// at the start of the active state, what was the mouse pos
    //VectorFloat2 _touch_pos_active_start; 
    /// The source token of the component that the touch started over, or the first source token found while active, to allow click drag onto element?
    void* _active_source_token;
    /// The touch id
    int _id;
    /// Duration of the touch, vaild while active or end is true
    float _active_duration;
    /// Mouse button down, or touch active
    bool _active;
    /// did the touch start this frame
    //bool _start; use null _active_source_token to allow click drag off element
    /// did the touch end this frame
    bool _end;
};

class UIRootInputState
{
public:
    UIRootInputState();

    void Update(
        const UIManagerDealInputParam& in_param,
        const VectorInt2& in_root_size
        );

    //const VectorFloat2& GetMousePosRef() const { return _mouse_pos; }
    //const bool GetMouseLeftDown() const { return _mouse_prev_left_down; }
    //void SetMouseLeftDownChange(const bool in_value) { _mouse_left_down_change = in_value; return; }
    //const bool GetMouseLeftDownChange() const { return _mouse_left_down_change; }

    std::vector<UIRootInputStateTouch>& GetTouchArray() { return _touch_array; }

private:
    std::vector<UIRootInputStateTouch> _touch_array;

    //// current focus, using unique pointer so no weak pointer, so just travers hierarchy to update flag?
    void* _focus_source_token;

    //void* _focus_node;

    //// node that click started on
    //// node that drag started from
    //// node being dragged

    //bool _mouse_valid;
    //int _mouse_prev_x;
    //int _mouse_prev_y;
    //int _mouse_delta_x;
    //int _mouse_delta_y;
    //bool _mouse_prev_left_down;
    //bool _mouse_left_down_change;

    //VectorFloat2 _mouse_pos;

};
