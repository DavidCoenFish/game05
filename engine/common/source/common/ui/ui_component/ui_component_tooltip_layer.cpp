#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_tooltip_layer.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/heap_wrapper/heap_wrapper_item.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_root_input_state.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/ui_data_tooltip_layer.h"

namespace
{
    /// TODO: when the tooltip is created, do you need to also calculate the tooltip pos there? slider example?
    /// touch and emitter are in [-1,-1 ... 1,1]
    const VectorFloat2 CalculateTooltipUV(const VectorFloat2& in_touch_pos, const VectorFloat4& in_emitter_screenspace, const UITooltipType in_tooltip_type)
    {
        in_touch_pos;
        in_emitter_screenspace;
        VectorFloat2 result;
        switch(in_tooltip_type)
        {
        default:
            break;
        case UITooltipType::TRelativeToTouch:
            result = VectorFloat2((in_emitter_screenspace[2] + in_emitter_screenspace[0]) * 0.5f, std::min(0.9f, in_emitter_screenspace[3]));
            break;
        case UITooltipType::TBottomLeft:
            result = VectorFloat2(0.25f, 0.1f);
            break;
        }
        return result;
    }
}

UIComponentTooltipLayer::UIComponentTooltipLayer(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
    : IUIComponent(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
{
    for (int index = 0; index < UIDataTooltipLayer::Variables::TMaxTooltipCount; ++index)
    {
        _tooltip_layout_target_array[index] = nullptr;
    }
    return;
}

UIComponentTooltipLayer::~UIComponentTooltipLayer()
{
    // Nop
}

void UIComponentTooltipLayer::RequestTooltip(
    const int in_index,
    const VectorFloat2& in_touch_pos, 
    const VectorFloat4& in_emitter_screenspace, 
    const std::string& in_text_run_string
    )
{
    if (_tooltip_type == UITooltipType::TNone)
    {
        return;
    }

    if (nullptr != _on_tooltip_change)
    {
        _on_tooltip_change(in_index, in_touch_pos, in_emitter_screenspace, in_text_run_string);
    }

    IUIComponent* const tooltip_layout_target = (0 <= in_index) && (in_index < UIDataTooltipLayer::Variables::TMaxTooltipCount) ?
        _tooltip_layout_target_array[in_index] : nullptr;
    if (nullptr != tooltip_layout_target)
    {
        if (true == tooltip_layout_target->SetStateFlagBit(UIStateFlag::THidden, false))
        {
            tooltip_layout_target->SetStateFlagBit(UIStateFlag::TDirty, true);
        }

        UILayout layout = tooltip_layout_target->GetLayout();
        const VectorFloat2 touch_pos = CalculateTooltipUV(in_touch_pos, in_emitter_screenspace, _tooltip_type); 
        layout.SetTooltip(touch_pos);

        tooltip_layout_target->SetLayoutOverride(layout);
    }
    return;
}

void UIComponentTooltipLayer::FinalizeTooltips(
    const std::vector<TooltipRequestData>& in_tooltip_request_data_array,
    const UIManagerDealInputParam& in_param
    )
{
    in_param;
    std::set<int> used_index;
    //std::set<void*> inuse_source_tokens;
    std::map<void*, int> map_active_tooltips;
    std::swap(map_active_tooltips, _map_active_tooltips);
    for (const auto& iter : in_tooltip_request_data_array)
    {
        auto found = map_active_tooltips.find(iter._source_token);
        if (found != map_active_tooltips.end())
        {
            //inuse_source_tokens.insert(found->first);
            used_index.insert(found->second);
        }
    }

    for (const auto& iter : in_tooltip_request_data_array)
    {
        bool add = false;
        int tooltip_index = 0;
        auto found = map_active_tooltips.find(iter._source_token);
        if (found != map_active_tooltips.end())
        {
            add = true;
            tooltip_index = found->second;
        }
        else
        {
            for (int index = 0; index < UIDataTooltipLayer::Variables::TMaxTooltipCount; ++index)
            {
                if (used_index.find(index) == used_index.end())
                {
                    add = true;
                    tooltip_index = index;
                    used_index.insert(index);
                    break;
                }
            }
        }

        if (true == add)
        {
            _map_active_tooltips[iter._source_token] = tooltip_index;
            RequestTooltip(
                tooltip_index,
                iter._touch_pos,
                iter._emitter_screenspace,
                iter._text_run_string
                );
        }
    }

    for (int index = 0; index < UIDataTooltipLayer::Variables::TMaxTooltipCount; ++index)
    {
        // skip over tooltips in use
        if (used_index.find(index) != used_index.end())
        {
            continue;
        }

        IUIComponent* const tooltip_layout_target = _tooltip_layout_target_array[index];
        if (nullptr != tooltip_layout_target)
        {
            if (true == tooltip_layout_target->SetStateFlagBit(UIStateFlag::THidden, true))
            {
                tooltip_layout_target->SetStateFlagBit(UIStateFlag::TDirty, true);
            }
        }
    }

    return;
}

const bool UIComponentTooltipLayer::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;

    // make sure hierarchy is populated first
    if (true == TSuper::UpdateHierarchy(
        in_data,
        in_out_child_data,
        in_param
        ))
    {
        dirty = true;
    }

#if 1
    UIDataTooltipLayer* const data = dynamic_cast<UIDataTooltipLayer*>(in_data);
    if (nullptr != data)
    {
        _on_tooltip_change = data->GetOnTooltipChange();
        _tooltip_scale = data->GetTooltipScale();
        // change of type could invalidata active tooltips? or wait for input pass to update
        _tooltip_type = data->GetTooltipType();

        const auto& touch_target_array = data->GetTooltipLayoutTargetArray();
        std::set<void*> map_touch_targets;
        for (auto item : touch_target_array)
        {
            auto target = item.get();
            if (nullptr != target)
            {
                map_touch_targets.insert(target);
            }
        }

        for (int index = 0; index < UIDataTooltipLayer::Variables::TMaxTooltipCount; ++index)
        {
            _tooltip_layout_target_array[index] = nullptr;
        }
        int found_count = 0;
        in_out_child_data.VisitComponents([&found_count, &map_touch_targets, this](IUIComponent* const in_component, UIHierarchyNode* const){
            if(nullptr == in_component)
            {
                return true;
            }
            void* source_token = in_component->GetSourceToken();
            auto found = map_touch_targets.find(source_token);
            if (found != map_touch_targets.end())
            {
                _tooltip_layout_target_array[found_count] = in_component;

                in_component->SetStateFlagBit(UIStateFlag::TLayoutOverride, true);
                in_component->SetStateFlagDirty(UIStateFlag::THidden);

                found_count += 1;
                if (UIDataTooltipLayer::Variables::TMaxTooltipCount <= found_count)
                {
                    return false;
                }
            }
            return true;
        });
    }
#endif
    return dirty;
}
