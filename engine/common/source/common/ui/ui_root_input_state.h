#pragma once

#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"

class IUIComponent;
class UIData;
class UIComponentTooltipLayer;
class VectorInt2;
struct UIManagerDealInputParam;

struct TooltipRequestData
{
    explicit TooltipRequestData(
        const VectorFloat2& in_touch_pos = VectorFloat2(),
        const VectorFloat4& in_emitter_screenspace = VectorFloat4(),
        const std::string& in_text_run_string = std::string(),
        void* const in_source_token = nullptr
        )
        : _touch_pos(in_touch_pos)
        , _emitter_screenspace(in_emitter_screenspace)
        , _text_run_string(in_text_run_string)
        , _source_token(in_source_token)
    {
        // Nop
    }
    VectorFloat2 _touch_pos;
    VectorFloat4 _emitter_screenspace;
    std::string _text_run_string;
    void* _source_token;
};

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
        const VectorInt2& in_root_size,
        UIData* const in_tooltip_layer_source_token
        );

    //const VectorFloat2& GetMousePosRef() const { return _mouse_pos; }
    //const bool GetMouseLeftDown() const { return _mouse_prev_left_down; }
    //void SetMouseLeftDownChange(const bool in_value) { _mouse_left_down_change = in_value; return; }
    //const bool GetMouseLeftDownChange() const { return _mouse_left_down_change; }

    std::vector<UIRootInputStateTouch>& GetTouchArray() { return _touch_array; }
    const float GetTimeDelta() const { return _time_delta; }

    void SubmitComponent(IUIComponent* const in_component);

    void RequestTooltip(
        const VectorFloat2& in_touch_pos, 
        const VectorFloat4& in_emitter_screenspace_clip, 
        const std::string& in_text_run_string,
        void* const in_source_token
        );

    /// Finialise tooltip, or does that just when we hit the UIComponentTooltipLayer, or is that the input to Finialise
    /// this may need UpdateHierarchy/size param? 
    void FinialiseTooltip(
        const UIManagerDealInputParam& in_param
        );

    //const std::vector<TooltipRequestData>& GetTooltipRequestData() const { return _tooltip_request_data; }

private:
    std::vector<UIRootInputStateTouch> _touch_array;

    /// current focus, using unique pointer so no weak pointer, so just traverse hierarchy to update flag?
    //void* _focus_source_token;
    float _time_delta;

    // cache the tooltip layer reference
    UIData* _tooltip_layer_source_token;
    UIComponentTooltipLayer* _tooltip_layer;

    std::vector<TooltipRequestData> _tooltip_request_data;

};
