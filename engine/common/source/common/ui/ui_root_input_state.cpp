#include "common/common_pch.h"
#include "common/ui/ui_root_input_state.h"
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
{
    // Nop
}

UIRootInputState::UIRootInputState()
    : _touch_array()
    , _focus_source_token(nullptr)
{
    // Nop
}

void UIRootInputState::Update(
    const UIManagerDealInputParam& in_param,
    const VectorInt2& in_root_size
    )
{
    _time_delta = in_param._time_delta;

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

