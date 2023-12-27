#include "common/common_pch.h"
#include "common/ui/ui_root_input_state.h"

//#include "common/ui/ui_component/ui_component_tooltip_layer.h"
//#include "common/ui/ui_component/ui_component_combo_box_layer.h"
#include "common/ui/ui_manager.h"
#include "common/log/log.h"
#include "common/math/dsc_math.h"

UIRootInputStateTouch::UIRootInputStateTouch(
    const VectorFloat2& in_touch_pos_current,
    void* const in_active_source_token,
    const int in_id,
    const float in_active_duration,
    const bool in_active,
    const bool in_end
    )
    : _touch_pos_current(in_touch_pos_current)
    , _active_source_token(in_active_source_token)
    , _id(in_id)
    , _active_duration(in_active_duration)
    , _active(in_active)
    //, _start(in_start)
    , _end(in_end)
    , _consumed(false)
{
    // Nop
}

UIRootInputState::UIRootInputState()
    : _touch_array()
    , _time_delta(0.0f)
    , _tooltip_layer_source_token(nullptr)
    , _tooltip_layer(nullptr)
    , _combo_box_layer_source_token(nullptr)
    , _combo_box_layer(nullptr)
    //, _focus_source_token(nullptr)
{
    // Nop
}

void UIRootInputState::Update(
    const UIManagerDealInputParam& in_param,
    const VectorInt2& in_root_size,
    UIData* const in_tooltip_layer_source_token,
    UIData* const in_combo_box_layer_source_token
    )
{
    _time_delta = in_param._time_delta;

    _tooltip_request_data.clear();
    _tooltip_layer = nullptr;
    _tooltip_layer_source_token = in_tooltip_layer_source_token;

    _combo_box_dropdown = nullptr;
    _combo_box_layer = nullptr;
    _combo_box_layer_source_token = in_combo_box_layer_source_token;

    std::map<int, UIRootInputStateTouch> touch_map;
    for (const auto iter : _touch_array)
    {
        touch_map[iter._id] = iter;
    }

    _touch_array.clear();
    for (const auto iter : in_param._touch_array)
    {
        const VectorFloat2 pos(
            DscMath::ConvertZeroOneToNegativeOneOne(in_root_size[0] ? static_cast<float>(iter._root_relative_pos[0]) / static_cast<float>(in_root_size[0]) : 0.0f),
            DscMath::ConvertZeroOneToNegativeOneOne(in_root_size[1] ? 1.0f - (static_cast<float>(iter._root_relative_pos[1]) / static_cast<float>(in_root_size[1])) : 0.0f)
            );
        //LOG_MESSAGE_DEBUG("%f %f", pos[0], pos[1]);

        auto found = touch_map.find(iter._id);
        if (found != touch_map.end())
        {
            _touch_array.push_back(found->second);

            auto& back = _touch_array.back();

            // mouse click finished, clear active source token
            if (true == back._end)
            {
                back._active_source_token = nullptr;
                back._active_duration = 0.0f;
                back._end = false;
            }

            back._touch_pos_current = pos;
            // mouse can turn off and on active, ie, click mouse button
            if (iter._active != back._active)
            {
                back._end = back._active;
            }
            if(true == back._active)
            {
                back._active_duration += in_param._time_delta;
            }
            back._active = iter._active;
            if ((false == back._active) && (false == back._end))
            {
                back._active_duration = 0.0f;
            }

            touch_map.erase(found);
        }
        else
        {
            _touch_array.push_back(
                UIRootInputStateTouch(
                    pos,
                    nullptr,
                    iter._id,
                    0.0f,
                    iter._active,
                    false
                    )
                );
        }

        // if we don't get told when a touch end is, just add it here
        for (auto item : touch_map)
        {
            if (false == item.second._end)
            {
                item.second._end = true;
                _touch_array.push_back(item.second);
            }
        }
    }

    return;
}

void UIRootInputState::SubmitComponent(IUIComponent* const in_component)
{
    if (nullptr == in_component)
    {
        return;
    }
#if 0
    if (_tooltip_layer_source_token == in_component->GetSourceToken())
    {
        _tooltip_layer = dynamic_cast<UIComponentTooltipLayer*>(in_component);
    }
    if (_combo_box_layer_source_token == in_component->GetSourceToken())
    {
        _combo_box_layer = dynamic_cast<UIComponentComboBoxLayer*>(in_component);
    }
#endif
    return;
}

void UIRootInputState::RequestTooltip(
    const VectorFloat2& in_touch_pos, 
    const VectorFloat4& in_emitter_screenspace_clip, 
    const std::string& in_text_run_string,
    void* const in_source_token
    )
{
    _tooltip_request_data.push_back(TooltipRequestData(
        in_touch_pos, 
        in_emitter_screenspace_clip, 
        in_text_run_string,
        in_source_token
        ));
}

// Finialise tooltip, or does that just when we hit the UIComponentTooltipLayer, or is that the input to Finialise
// this may need UpdateHierarchy/size param? 
void UIRootInputState::FinialiseTooltip()
{
    //if (nullptr != _tooltip_layer)
    //{
    //    _tooltip_layer->FinalizeTooltips(
    //        _tooltip_request_data
    //        );
    //}
    return;
}

void UIRootInputState::RequestComboBoxDropdown(
    std::shared_ptr<UIHierarchyNodeChild>& in_dropdown
    )
{
    _combo_box_dropdown = in_dropdown;
    return;
}

void UIRootInputState::FinialiseComboBox()
{
    //if ((nullptr != _combo_box_layer) && (nullptr != _combo_box_dropdown))
    //{
    //    _combo_box_layer->TriggerComboBoxDropDown(_combo_box_dropdown);
    //}
    return;
}
