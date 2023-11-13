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
    : IUIComponent(
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

const bool UIComponentListBox::UpdateHierarchy(
    UIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;

    if (true == TSuper::UpdateHierarchy(
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
        in_out_child_data.VisitComponents([this, &source_token_list_box_index, selection_index](IUIComponent* const in_component, UIHierarchyNode* const in_node){
            if (nullptr != in_component)
            {
                auto found = source_token_list_box_index.find(in_component->GetSourceToken());
                if (found != source_token_list_box_index.end())
                {
                    const int found_index = found->second;
                    const bool is_selected = selection_index == found_index;
                    if (true == in_component->SetStateFlagBit(UIStateFlag::TSelected, is_selected))
                    {
                        if (nullptr != in_node)
                        {
                            in_node->MarkTextureDirty();
                        }
                    }

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
