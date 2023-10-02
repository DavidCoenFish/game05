#pragma once

class VectorFloat2;

enum class UIInputMouseAction
{
    TNone,
    TLeftDown,
    TDrag,
    TClick, // Left up on the same element that got the left down
    TDiscard, // Left up off element
    TDrop, // Left up over this element, but it didn't have the left down
    THover
};

class IUIInput
{
public:
    virtual ~IUIInput();

    virtual void OnMouse(
        const UIInputMouseAction in_action,
        const VectorFloat2& in_screen_pos_or_delta //
        ) = 0;

};
