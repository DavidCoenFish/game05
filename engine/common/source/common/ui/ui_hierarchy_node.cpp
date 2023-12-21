#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/ui/i_ui_input.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_effect/ui_effect_component.h"
#include "common/ui/ui_effect/ui_effect_data.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_enum.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node_child.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_root_input_state.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_texture.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

UIHierarchyNodeUpdateParam::UIHierarchyNodeUpdateParam(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    UIManager* const in_ui_manager,
    LocaleSystem* const in_locale_system,
    TextManager* const in_text_manager,
    const UIDataTextRunStyle* const in_default_text_style,
    const float in_ui_scale,
    const float in_delta_time_seconds
    )
    : _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_manager(in_ui_manager)
    , _locale_system(in_locale_system)
    , _text_manager(in_text_manager)
    , _default_text_style(in_default_text_style)
    , _ui_scale(in_ui_scale)
    , _delta_time_seconds(in_delta_time_seconds)
{
    // Nop
}

UIHierarchyNode::UIHierarchyNode()
{
    _texture = std::make_unique<UITexture>();

}

UIHierarchyNode::~UIHierarchyNode()
{
    // Nop
}

UIRootInputState& UIHierarchyNode::GetOrMakeRootInputState()
{
    if (nullptr == _root_input_state)
    {
        _root_input_state = std::make_unique<UIRootInputState>();
    }
    return *_root_input_state;
}

//void UIHierarchyNode::AddChild(
//    std::unique_ptr<IUIComponent>& in_content
//    )
//{
//    auto geometry = std::make_unique<UIGeometry>();
//    auto node = std::make_unique<UIHierarchyNode>();
//    auto screen_space = std::make_unique<UIScreenSpace>();
//    auto child = UIHierarchyNodeChild::Factory(
//    
//    
//    std::make_shared<UIHierarchyNodeChild>(
//        geometry,
//        in_content,
//        node,
//        screen_space
//        );
//    _child_array.push_back(child);
//}
//
//const bool UIHierarchyNode::ClearChildren()
//{
//    bool dirty = false;
//    if (0 != _child_array.size())
//    {
//        dirty = true;
//        _child_array.clear();
//    }
//    return dirty;
//}

#if 1
// return true if the parent should have their texture dirtied, moved to node child state flag
void UIHierarchyNode::UpdateHierarchy(
    UIData& in_data,
    UIHierarchyNodeChild* const in_parent_to_this_node_or_null,
    const UIHierarchyNodeUpdateParam& in_param,
    const int in_child_index
    )
{
    // dirty is for if the texture needs to be redrawn. issue, we want higher fidelity of the dirty flag, ie, layout, resource updates
    //bool dirty = false;

    bool layout_dirty = false;
    bool render_dirty = false;

    // Sync texture. 
    DSC_ASSERT(nullptr != _texture, "ui texture should be created in ctor, what happened");
    if (true == in_data.GetDirtyBit(UIDataDirty::TTextureDirty))
    {
        const auto& base_colour = in_data.GetBaseColour();
        if (true == _texture->Update(
            base_colour.GetDrawToTexture(),
            base_colour.GetAlwaysDirty(),
            base_colour.GetClearBackground(),
            base_colour.GetClearColourRef()
            ))
        {
            render_dirty = true;
        }
        in_data.SetDirtyBit(UIDataDirty::TTextureDirty, false);
    }

    if (true == in_data.GetDirtyBit(UIDataDirty::TComponentDirty))
    {
        if (nullptr != in_parent_to_this_node_or_null)
        {
            // ensure component is the correct type
            in_parent_to_this_node_or_null->ApplyComponent(
                in_data,
                in_param,
                in_child_index
                );
        }

        in_data.SetDirtyBit(UIDataDirty::TComponentDirty, false);
    }

    if (true == in_data.GetDirtyBit(UIDataDirty::THierarchyDirty))
    {
        auto& ui_data_child_data = in_data.GetArrayChildData();
        if (0 == ui_data_child_data.size())
        {
            if (0 != _child_array.size())
            {
                _child_array.clear();
                render_dirty = true; // removing children could effect the render result of the parent...
            }
        }
        else
        {
            std::map<void*, std::shared_ptr<UIHierarchyNodeChild>> map_temp_ownership;

            // if there are too many items, put the remaining into the temp owership map
            const int target_length = static_cast<int>(ui_data_child_data.size());
            for (int index = target_length; index < static_cast<int>(_child_array.size()); ++index)
            {
                auto child = _child_array[index];
                map_temp_ownership[child->GetSourceToken()] = child;
                render_dirty = true;
            }
            _child_array.resize(target_length);

            // Remove mismatched items in child data array
            for (int index = 0; index < target_length; ++index)
            {
                const auto& data = *(ui_data_child_data[index]);
                const auto& child = _child_array[index];
                if (nullptr != child)
                {
                    void* source_child_token = child ? child->GetSourceToken() : nullptr;
                    void* data_token = (void*)&data;
                    if (data_token != source_child_token)
                    {
                        if (nullptr != source_child_token)
                        {
                            map_temp_ownership[source_child_token] = child;
                        }
                        _child_array[index].reset();
                        render_dirty = true;
                    }
                    // do this in data apply component
                    //else
                    //{
                    //    auto& child_ref = *child;
                    //    child_ref.SetLayout(data.GetLayout());
                    //    child_ref.SetTintColour(data.GetTintColour());
                    //}
                }
            }

            // Find / create missing children
            for (int index = 0; index < target_length; ++index)
            {
                if (nullptr != _child_array[index])
                {
                    continue;
                }
                auto& data = *(ui_data_child_data[index]);
                void* data_token = (void*)&data;
                auto found = map_temp_ownership.find(data_token);
                if (found != map_temp_ownership.end())
                {
                    _child_array[index] = found->second;
                    map_temp_ownership.erase(found);
                }
                else
                {
                    _child_array[index] = UIHierarchyNodeChild::Factory(
                        in_parent_to_this_node_or_null,
                        data.GetLayout(),
                        data.GetTintColour(),
                        &data
                        );
                }
                layout_dirty = true;
                render_dirty = true;
            }
        }

        in_data.SetDirtyBit(UIDataDirty::THierarchyDirty, false);
    }

    if (true == in_data.GetDirtyBit(UIDataDirty::TEffectDirty))
    {
        if (true == ApplyEffect(
            in_data.GetArrayEffectData(),
            in_param,
            in_child_index
            ))
        {
            render_dirty = true; // if the effect stack data changes, that just effects the render
        }

        in_data.SetDirtyBit(UIDataDirty::TEffectDirty, false);
    }

    // update state flags
    if (nullptr != in_parent_to_this_node_or_null)
    {
        if (true == layout_dirty)
        {
            in_parent_to_this_node_or_null->SetStateDirtyBit(UIStateDirty::TLayoutDirty, true);
        }
        if (true == render_dirty)
        {
            in_parent_to_this_node_or_null->SetStateDirtyBit(UIStateDirty::TRenderDirty, true);
        }
    }

    // only recursive if decendant dirty
    if (true == in_data.GetDirtyBit(UIDataDirty::TDecendantDirty))
    {
        for (int index = 0; index < in_data.GetArrayChildData().size(); ++index)
        {
            UIData* const child_data = in_data.GetArrayChildData()[index].get();
            UIHierarchyNodeChild* const child = _child_array[index].get();
            child->UpdateHierarchy(
                *child_data,
                in_param,
                index
                );
        }

        in_data.SetDirtyBit(UIDataDirty::TDecendantDirty, false);
    }

    return;
}

#else
void UIHierarchyNode::UpdateHierarchy(
    const UIHierarchyNodeUpdateParam& in_param,
    const std::vector<std::shared_ptr<UIData>>& ui_data_child_data,
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    const int in_child_index,
    const bool in_dirty,
    const bool in_render_to_texture,
    const bool in_always_dirty,
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
{
    bool dirty = in_dirty; //false;

    // Update texture
    if (nullptr == _texture)
    {
        _texture = std::make_unique<UITexture>(
            in_render_to_texture,
            in_always_dirty,
            in_allow_clear,
            in_clear_colour
            );
        dirty = true;
    }
    else
    {
        if (true == _texture->Update(
            in_render_to_texture,
            in_always_dirty,
            in_allow_clear,
            in_clear_colour
            ))
        {
            dirty = true;
        }
    }

    // Update child data array
    if (0 == ui_data_child_data.size())
    {
        if (0 != _child_array.size())
        {
            _child_array.clear();
            dirty = true;
        }
    }
    else
    {
        std::map<void*, std::shared_ptr<UIHierarchyNodeChild>> map_temp_ownership;

        // if there are too many items, put the remaining into the temp owership map
        const int target_length = static_cast<int>(ui_data_child_data.size());
        for (int index = target_length; index < static_cast<int>(_child_array.size()); ++index)
        {
            auto child = _child_array[index];
            map_temp_ownership[child->GetSourceToken()] = child;
            dirty = true;
        }
        _child_array.resize(target_length);

        // Remove mismatched items in child data array
        for (int index = 0; index < target_length; ++index)
        {
            const auto& data = *(ui_data_child_data[index]);
            const auto& child = _child_array[index];
            if (nullptr != child)
            {
                void* source_child_token = child->_component ? child->GetSourceToken() : nullptr;
                void* data_token = (void*)&data;
                if (data_token != source_child_token)
                {
                    if (nullptr != source_child_token)
                    {
                        map_temp_ownership[source_child_token] = child;
                    }
                    _child_array[index].reset();
                    dirty = true;
                }
            }
        }

        // Find / create missing children
        for (int index = 0; index < target_length; ++index)
        {
            if (nullptr != _child_array[index])
            {
                continue;
            }
            auto& data = *(ui_data_child_data[index]);
            void* data_token = (void*)&data;
            auto found = map_temp_ownership.find(data_token);
            if (found != map_temp_ownership.end())
            {
                _child_array[index] = found->second;
                map_temp_ownership.erase(found);
            }
            else
            {
                _child_array[index] = UIHierarchyNodeChild::Factory();
                dirty = true;
                // createing a node means it doesn't have the correct layout
                data.SetDirtyBit(UIDataDirty::THierarchy, true);
                data.SetDirtyBit(UIDataDirty::TLayout, true);
            }
        }

        // ensure each content is created/ passed through factory
        for (int index = 0; index < target_length; ++index)
        {
            UIData* const data = ui_data_child_data[index].get();
            UIHierarchyNodeChild* const child = _child_array[index].get();

            if (nullptr == data)
            {
                child->_component.reset();
            }
            else 
            {
                // ensure component is the correct type
                data->ApplyComponent(
                    child->_component,
                    in_param
                    );

                if (true == data->GetDirtyBit(UIDataDirty::TDecendantDirty))
                {
                    // recurse
                    child->_node->UpdateHierarchy(
                        in_param,
                        data->GetArrayChildData(),
                        data->GetArrayEffectData(),
                        index,
                        in_dirty, 
                        data->GetBaseColour().GetDrawToTexture(),
                        data->GetBaseColour().GetAlwaysDirty(),
                        data->GetBaseColour().GetClearBackground(),
                        data->GetBaseColour().GetClearColourRef()
                        );
                    data->SetDirtyBit(UIDataDirty::TDecendantDirty, false);
                }
            }
        }
    }

    if (true == ApplyEffect(
        in_array_effect_data,
        in_param,
        in_child_index
        ))
    {
        dirty = true;
    }

    if ((true == dirty) && (nullptr != _texture))
    {
        _texture->MarkDirty();
    }

    return;
}
#endif

/// node::update layout (in parent window)
///     for each child A0
///         component::update layout
///             calculate layout size given parent window
///             recurse node::update layout
///             component::desired(parent window or layout size? layout size may yet to be modified...)
///             finialise layout size (shrink/expand)
void UIHierarchyNode::UpdateLayout(
    const UIHierarchyNodeUpdateParam& in_param,
    const VectorInt2& in_parent_window,
    const VectorInt2& in_parent_offset
    )
{
    for (auto& child : _child_array)
    {
        DSC_ASSERT(nullptr != child, "confirm that children can not be null");
        child->UpdateLayout(
            in_param,
            in_parent_window,
            in_parent_offset
            );
    }

    return;
}

void UIHierarchyNode::UpdateResources(
    const UIHierarchyNodeUpdateParam& in_param,
    const UIScreenSpace& in_parent_screen_space
    )
{
    const VectorInt2 texture_size = _texture->GetSize(in_param._draw_system);

    for (auto& child : _child_array)
    {
        DSC_ASSERT(nullptr != child, "confirm that children can not be null");
        child->UpdateResources(
            in_param,
            in_parent_screen_space,
            texture_size
            );
    }

    for (auto& effect : _array_effect_component)
    {
        effect->Update(
            in_param,
            texture_size
            );
    }
    return;
}

void UIHierarchyNode::MarkTextureDirty()
{
    _texture->MarkDirty();
    return;
}

const VectorInt2 UIHierarchyNode::GetTextureSize(
    DrawSystem* const in_draw_system
    ) const
{
    return _texture->GetSize(in_draw_system);
}

void UIHierarchyNode::SetTextureSize(
    const VectorInt2& in_texture_size
    )
{
    _texture->SetSize(in_texture_size);
}

// TODO: at some point need to collect "input" elements as potential navigation targets/ resolve navigation keypress
const bool UIHierarchyNode::DealInput(
    UIRootInputState&,// in_input_state,
    const bool,// in_parent_inside,
    const bool// in_action
    )
{
    bool dirty = false;
/*
    for(auto& child_data_ptr : _child_array)
    {
        UIHierarchyNodeChild& child_data = *child_data_ptr;

        int local_flag = 0; //in_state_flag;
        void* source_token = nullptr;
        IUIComponent* const component = child_data._component.get();
        if (nullptr != component)
        {
             local_flag = static_cast<int>(component->GetStateFlag()) & ~static_cast<int>(UIStateFlag::TMaskInput);
             source_token = component->GetSourceToken();
        }

        // nope out of disabled components, don't even recurse
        if ((0 != (local_flag & static_cast<int>(UIStateFlag::TDisable))) ||
            (0 != (local_flag & static_cast<int>(UIStateFlag::THidden))))
        {
            continue;
        }

        IUIInput* const input = dynamic_cast<IUIInput*>(component);
        in_input_state.SubmitComponent(component);

        bool inside = false;
        bool action = nullptr == input ? in_action : false;
        bool hover = false;

        // update state flag for all touches
        for (auto& touch : in_input_state.GetTouchArray())
        {
            std::string tooltip;
            const bool local_inside = child_data._screen_space->GetClipRef().Inside(touch._touch_pos_current);

            if ((true == in_parent_inside) && 
                (true == local_inside))
            {
                inside = true;

                if ((nullptr == touch._active_source_token) || 
                    (touch._active_source_token == source_token))
                {
                    hover = true;
                    if (nullptr != input)
                    {
                        input->OnHover(child_data._screen_space->GetPosRef(), touch._touch_pos_current, tooltip);
                    }
                }
            }

            // OnDrag, drag slider, can be outside, but protected by matching source token
            if ((true == touch._active) &&
                (nullptr != touch._active_source_token) &&
                (nullptr != input) &&
                (touch._active_source_token == source_token) &&
                (false == touch._end))
            {
                input->OnInputTouch(child_data._screen_space->GetPosRef(), touch._touch_pos_current, tooltip);
            }

            // Click and hold button (repeat)
            if ((true == local_inside) &&
                (nullptr != touch._active_source_token) &&
                (touch._active_source_token == source_token) &&
                (nullptr != input))
            {
                action = true;
                input->OnInputRepeat(
                    child_data._screen_space->GetPosRef(),
                    touch._touch_pos_current,
                    touch._active_duration,
                    in_input_state.GetTimeDelta(),
                    tooltip
                    );
            }

            // OnClick, needs to be over the same element as touch started on
            if ((true == touch._end) &&
                (true == in_parent_inside) &&
                (true == local_inside) &&
                (nullptr != touch._active_source_token) &&
                (nullptr != input) &&
                (touch._active_source_token == source_token))
            {
                input->OnInputClick(
                    in_input_state,
                    child_data._screen_space->GetPosRef(),
                    touch._touch_pos_current
                    );
                //touch._end = false; // consume input, is done by matching source_token
            }

            if (false == tooltip.empty())
            {
                in_input_state.RequestTooltip(touch._touch_pos_current, child_data._screen_space->GetClipRef(), tooltip, source_token);
            }
        }

        // recurse
        if (true == child_data_ptr->_node->DealInput(
            in_input_state,
            inside,
            action
            ))
        {
            _texture->MarkDirty();
        }

        // deal click, set active if not consumed during eralier recursion 
        // choose first hit deepest element for first active
        for (auto& touch : in_input_state.GetTouchArray())
        {
            const bool local_inside = child_data._screen_space->GetClipRef().Inside(touch._touch_pos_current);

            // this is the first active source token for this touch
            if ((nullptr != input) &&
                (true == touch._active) &&
                (nullptr == touch._active_source_token) &&
                (true == in_parent_inside) && 
                (true == local_inside))
            {
                touch._active_source_token = source_token;
                //touch._touch_pos_active_start = touch._touch_pos_current;
            }

        }

        if (true == hover) //inside)
        {
            local_flag |= static_cast<int>(UIStateFlag::THover);
        }

        if (true == action)
        {
            local_flag |= static_cast<int>(UIStateFlag::TTouch);
        }

        if (nullptr != component)
        {
             if (true == component->SetStateFlag(static_cast<UIStateFlag>(local_flag)))
             {
                child_data._node->MarkTextureDirty();
                dirty = true;
             }
        }

        // alternative to casting every component to a tooltip layer is making a seperate UIManager method/extrnally having a reference to the tooltip layer to hand the input state?
        //UIComponentTooltipLayer* const tooltip_layer = dynamic_cast<UIComponentTooltipLayer*>(component);
        //if (nullptr != tooltip_layer)
        //{
        //    in_input_state.FinialiseTooltip(*tooltip_layer);
        //}
    }
*/
    return dirty;
}

// Return True if we needed to draw, ie, the texture may have changed
void UIHierarchyNode::PreDraw(
    const UIManagerDrawParam& in_draw_param
    )
{
    // Ensure that children textures are ready
    for (auto& iter : _child_array)
    {
        iter->PreDraw(in_draw_param);
    }

    return;
}

void UIHierarchyNode::Draw(
    const UIManagerDrawParam& in_draw_param//,
    //const bool in_dirty,
    , const UIStateFlag in_state_flag
    )
{
    bool child_needs_draw = false;
    for (auto iter : _child_array)
    {
        if (true == iter->GetStateDirtyBit(UIStateDirty::TRenderDirty))
        {
            child_needs_draw = true;
            break;
        }
    }

    if ((true == child_needs_draw) ||
        (true == _texture->CalculateNeedsToDraw()))
    {
        if (false == _texture->SetRenderTarget(
            in_draw_param._draw_system,
            in_draw_param._frame
            ))
        {
            return;
        }

        for (auto& iter : _child_array)
        {
            UIHierarchyNodeChild& child_data = *iter;

            child_data.Draw(in_draw_param);
        }


        if (0 < _array_effect_component.size())
        {
            UITexture* trace_texture = _texture.get();
            for (auto& effect: _array_effect_component)
            {
                effect->Render(
                    in_draw_param,
                    *trace_texture,
                    in_state_flag
                    );
                trace_texture = &effect->GetUITexture();
            }
        }

        _texture->SetHasDrawn(true);
    }

    return;
}

UITexture& UIHierarchyNode::GetUITexture() const
{ 
    if (0 < _array_effect_component.size())
    {
        return _array_effect_component.back()->GetUITexture();
    }
    return *_texture; 
}

const bool UIHierarchyNode::ApplyEffect(
    const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
    const UIHierarchyNodeUpdateParam& in_param,
    const int in_index
    )
{
    bool change = false;
    const int size = static_cast<int>(in_array_effect_data.size());
    if (size != _array_effect_component.size())
    {
        change = true;
        _array_effect_component.resize(size);
    }

    for (int index = 0; index < size; ++index)
    {
        if (true == in_array_effect_data[index]->ApplyComponent(
            _array_effect_component[index],
            in_param,
            in_index
            ))
        {
            change = true;
        }
    }

    return change;
}
