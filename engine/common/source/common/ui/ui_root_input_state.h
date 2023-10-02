#pragma once

class UIRootInputState
{
public:
    UIRootInputState();

    void Update(const UIManagerDealInputParam& in_param);

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

};
