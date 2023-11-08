#include "common/common_pch.h"
#include "common/ui/ui_component/ui_component_list_box.h"

#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/log/log.h"
#include "common/ui/ui_component/ui_component_scroll.h"
#include "common/ui/ui_component/ui_component_button.h"
#include "common/ui/ui_data/ui_data_list_box.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_enum.h"

UIComponentListBox::UIComponentListBox(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
    : _component_default(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        )
    //, _selection_index(0)
{
    // Nop
}

UIComponentListBox::~UIComponentListBox()
{
    // Nop
}

const bool UIComponentListBox::SetBase(
    const UIBaseColour& in_base_colour,
    const UILayout& in_layout,
    const std::shared_ptr<const TStateFlagTintArray>& in_state_flag_tint_array
    )
{
    bool dirty = false;
    if (true == _component_default.SetBase(
        in_base_colour,
        in_layout,
        in_state_flag_tint_array
        ))
    {
        dirty = true;
    }

    return dirty;
}

/// inpoint for item button callbacks
void UIComponentListBox::SetSelectedIndex(const int in_selected_index)
{
    //_selection_index = in_selected_index;
    if (nullptr != _on_selection_change)
    {
        _on_selection_change(in_selected_index);
    }
    return;
}

const bool UIComponentListBox::SetStateFlag(const UIStateFlag in_state_flag)
{
    bool dirty = false;
    if (true == _component_default.SetStateFlag(in_state_flag))
    {
        dirty = true;
    }

    return dirty;
}

const bool UIComponentListBox::SetStateFlagBit(const UIStateFlag in_state_flag_bit, const bool in_enable)
{
    return _component_default.SetStateFlagBit(in_state_flag_bit, in_enable);
}

const UIStateFlag UIComponentListBox::GetStateFlag() const
{
    return _component_default.GetStateFlag();
}

const UILayout& UIComponentListBox::GetLayout() const
{
    return _component_default.GetLayout();
}

void UIComponentListBox::SetLayoutOverride(const UILayout& in_override)
{
    _component_default.SetLayoutOverride(in_override);
    return;
}

void UIComponentListBox::SetUVScrollManual(const VectorFloat2& in_uv_scroll, const bool in_manual_horizontal, const bool in_manual_vertical)
{
    _component_default.SetUVScrollManual(in_uv_scroll, in_manual_horizontal, in_manual_vertical);
    return;
}

void UIComponentListBox::SetSourceToken(void* in_source_ui_data_token)
{
    _component_default.SetSourceToken(in_source_ui_data_token);
    return;
}

void* UIComponentListBox::GetSourceToken() const
{
    return _component_default.GetSourceToken();
}

const bool UIComponentListBox::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;

    if (true == _component_default.UpdateHierarchy(
        in_data,
        in_out_child_data, 
        in_param
        ))
    {
        dirty = true;
    }

    std::map<void*, int> source_token_list_box_index;
    const UIDataListBox* const data = dynamic_cast<const UIDataListBox*>(in_data);
    int selection_index = 0;
    if (nullptr != data)
    {
        _on_selection_change = data->GetOnSelectedIndexChange();
        selection_index = data->GetSelectedIndex();
        //_selection_index = data->GetSelectedIndex();
        const auto& item_button_array = data->GetItemHostButtonArrayRef();
        for (int index = 0; index < static_cast<int>(item_button_array.size()); ++index)
        {
            source_token_list_box_index[item_button_array[index].get()] = index;
        }
    }

    if (0 < source_token_list_box_index.size())
    {
        in_out_child_data.VisitComponents([this, &source_token_list_box_index, selection_index](IUIComponent* const in_component){
            if (nullptr != in_component)
            {
                auto found = source_token_list_box_index.find(in_component->GetSourceToken());
                if (found != source_token_list_box_index.end())
                {
                    const int found_index = found->second;
                    const bool is_selected = selection_index == found_index;
                    in_component->SetStateFlagBit(UIStateFlag::TSelected, is_selected);

                    // alternative is to make a UIComponentListBoxItem rather than UIComponentButton?
                    // improvement would be to throttle how often hierarchy update is called, as we now create a lambda for every item every update
                    UIComponentButton* const button = dynamic_cast<UIComponentButton*>(in_component);
                    if (button)
                    {
                        button->Set(
                            [this, found_index](const VectorFloat2&){
                                this->SetSelectedIndex(found_index);
                            },
                            false
                            );
                    }
                }
            }
            return true;
        });
    }

    return dirty;
}

const bool UIComponentListBox::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node, // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    const UIScreenSpace& in_parent_screen_space,
    UIScreenSpace& out_screen_space
    )
{
    return _component_default.UpdateSize(
        in_draw_system,
        *this,
        in_parent_size,
        in_parent_offset,
        in_parent_window,
        in_ui_scale, 
        in_time_delta,
        in_out_geometry, 
        in_out_node,
        in_parent_screen_space,
        out_screen_space
        );
}

void UIComponentListBox::GetDesiredSize(
    VectorInt2& out_layout_size, // if layout has shrink enabled, and desired size was smaller than layout size, the layout size can shrink
    VectorInt2& out_desired_size, // if bigger than layout size, we need to scroll
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    _component_default.GetDesiredSize(
        out_layout_size,
        out_desired_size,
        in_parent_window,
        in_ui_scale,
        in_out_node
        );
    return;
}

const bool UIComponentListBox::PreDraw(
    const UIManagerDrawParam& in_draw_param,
    UIHierarchyNode& in_node
    ) 
{
    return _component_default.PreDraw(
        in_draw_param,
        in_node
        );
}

const VectorFloat4 UIComponentListBox::GetTintColour() const
{
    return _component_default.GetTintColour();
}
