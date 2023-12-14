#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/ui/i_ui_input.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_effect/ui_effect_component.h"
#include "common/ui/ui_effect/ui_effect_data.h"

#include "common/ui/ui_coord.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_root_input_state.h"
#include "common/ui/ui_base_colour.h"
#include "common/ui/ui_enum.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

std::shared_ptr<UIHierarchyNodeChildData> UIHierarchyNodeChildData::Factory()
{
    auto geometry = std::make_unique<UIGeometry>();
    std::unique_ptr<IUIComponent> component;
    auto node = std::make_unique<UIHierarchyNode>();
    auto screen_space = std::make_unique<UIScreenSpace>();
    return std::make_shared<UIHierarchyNodeChildData>(
        geometry,
        component,
        node,
        screen_space
        );
}

UIHierarchyNodeChildData::UIHierarchyNodeChildData(
    std::unique_ptr<UIGeometry>& in_geometry,
    std::unique_ptr<IUIComponent>& in_component,
    std::unique_ptr<UIHierarchyNode>& in_node,
    std::unique_ptr<UIScreenSpace>& in_screen_space
    )
    : _geometry(std::move(in_geometry))
    , _component(std::move(in_component))
    , _node(std::move(in_node))
    , _screen_space(std::move(in_screen_space))
{
    // Nop
}

void UIHierarchyNodeChildData::Draw(const UIManagerDrawParam& in_draw_param)
{
    const auto& shader = in_draw_param._ui_manager->GetShaderRef(UIShaderEnum::TDefault);

    _node->GetUITexture().SetShaderResource(
        *shader,
        0,
        in_draw_param._frame
        );

    if (nullptr == _shader_constant_buffer)
    {
        _shader_constant_buffer = shader->MakeShaderConstantBuffer(
            in_draw_param._draw_system
            );
    }

    if (nullptr != _shader_constant_buffer)
    {
        UIManager::TShaderConstantBuffer& buffer = _shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
        buffer._tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    in_draw_param._frame->SetShader(shader, _shader_constant_buffer);

    _geometry->Draw(
        in_draw_param._draw_system,
        in_draw_param._frame
        );

    return;
}

const bool UIHierarchyNodeChildData::VisitComponents(const std::function<const bool(IUIComponent* const, UIHierarchyNode* const)>& in_visitor)
{
    bool keep_going = true;
    auto* component = _component.get();
    if (nullptr != component)
    {
        keep_going = in_visitor(component, _node.get());
    }
    if (true == keep_going)
    {
        for (auto iter : _node->GetChildData())
        {
            keep_going = iter->VisitComponents(in_visitor);
            if (false == keep_going)
            {
                break;
            }
        }
    }

    return keep_going;
}

/*
const bool UIHierarchyNodeChildData::RecurseSetStateFlagInput(UIHierarchyNodeChildData* const in_data, const UIStateFlag in_state_flag)
{
    bool dirty = false;

    if (nullptr == in_data)
    {
        return dirty;
    }

    IUIComponent* const component = in_data->_component.get();
    if (nullptr != component)
    {
        int state_flag = static_cast<int>(component->GetStateFlag());
        state_flag &= ~static_cast<int>(UIStateFlag::TMaskInput);
        state_flag |= (static_cast<int>(in_state_flag) & static_cast<int>(UIStateFlag::TMaskInput));
        if (true == component->SetStateFlag(static_cast<UIStateFlag>(state_flag)))
        {
            dirty = true;
        }
    }

    UIHierarchyNode* const node = in_data->_node.get();
    if (nullptr != node)
    {
        for (auto iter : node->GetChildData())
        {
            if (true == RecurseSetStateFlagInput(iter.get(), in_state_flag))
            {
                dirty = true;
            }
        }
    }

    return dirty;
}
*/

UIHierarchyNodeChildData::~UIHierarchyNodeChildData()
{
    // Nop
}

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

void UIHierarchyNode::AddChild(
    std::unique_ptr<IUIComponent>& in_content
    )
{
    auto geometry = std::make_unique<UIGeometry>();
    auto node = std::make_unique<UIHierarchyNode>();
    auto screen_space = std::make_unique<UIScreenSpace>();
    auto child = std::make_shared<UIHierarchyNodeChildData>(
        geometry,
        in_content,
        node,
        screen_space
        );
    _child_data_array.push_back(child);
}

const bool UIHierarchyNode::ClearChildren()
{
    bool dirty = false;
    if (0 != _child_data_array.size())
    {
        dirty = true;
        _child_data_array.clear();
    }
    return dirty;
}

void UIHierarchyNode::UpdateHierarchy(
    const UIHierarchyNodeUpdateParam& in_param,
    const std::vector<std::shared_ptr<UIData>>& in_array_child_data,
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
    if (0 == in_array_child_data.size())
    {
        if (0 != _child_data_array.size())
        {
            _child_data_array.clear();
            dirty = true;
        }
    }
    else
    {
        std::map<void*, std::shared_ptr<UIHierarchyNodeChildData>> map_temp_ownership;

        // if there are too many items, put the remaining into the temp owership map
        const int target_length = static_cast<int>(in_array_child_data.size());
        for (int index = target_length; index < static_cast<int>(_child_data_array.size()); ++index)
        {
            auto child = _child_data_array[index];
            map_temp_ownership[child->_component->GetSourceToken()] = child;
            dirty = true;
        }
        _child_data_array.resize(target_length);

        // Remove mismatched items in child data array
        for (int index = 0; index < target_length; ++index)
        {
            const auto& data = *(in_array_child_data[index]);
            const auto& child = _child_data_array[index];
            if (nullptr != child)
            {
                void* source_child_token = child->_component ? child->_component->GetSourceToken() : nullptr;
                void* data_token = (void*)&data;
                if (data_token != source_child_token)
                {
                    if (nullptr != source_child_token)
                    {
                        map_temp_ownership[source_child_token] = child;
                    }
                    _child_data_array[index].reset();
                    dirty = true;
                }
            }
        }

        // Find / create missing children
        for (int index = 0; index < target_length; ++index)
        {
            if (nullptr != _child_data_array[index])
            {
                continue;
            }
            auto& data = *(in_array_child_data[index]);
            void* data_token = (void*)&data;
            auto found = map_temp_ownership.find(data_token);
            if (found != map_temp_ownership.end())
            {
                _child_data_array[index] = found->second;
                map_temp_ownership.erase(found);
            }
            else
            {
                _child_data_array[index] = UIHierarchyNodeChildData::Factory();
                dirty = true;
                // createing a node means it doesn't have the correct layout
                data.SetDirtyBit(UIDataDirty::THierarchy, true);
                data.SetDirtyBit(UIDataDirty::TLayout, true);
            }
        }

        // ensure each content is created/ passed through factory
        for (int index = 0; index < target_length; ++index)
        {
            UIData* const data = in_array_child_data[index].get();
            UIHierarchyNodeChildData* const child = _child_data_array[index].get();

            if (nullptr == data)
            {
                child->_component.reset();
            }
            else if ((true == in_dirty) || (true == data->GetDirtyBit(UIDataDirty::THierarchy)))
            {
                // ensure component is the correct type
                data->ApplyComponent(
                    child->_component,
                    in_param
                    );

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

                data->SetDirtyBit(UIDataDirty::THierarchy, false);
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
    for (auto& child : _child_data_array)
    {
        IUIComponent* component = child ? child->_component.get() : nullptr;
        if (nullptr == component)
        {
            continue;
        }

        if (true == component->GetStateFlagBit(UIStateFlag::TLayoutDirty))
        {
            component->UpdateLayout(
                *child,
                in_param,
                in_parent_window,
                in_parent_offset
                );
            component->SetStateFlagBit(UIStateFlag::TLayoutDirty, false);
        }
    }

    return;
}

const bool UIHierarchyNode::UpdateResources(
    const UIHierarchyNodeUpdateParam& in_param,
    const UIScreenSpace& in_parent_screen_space
    )
{
    bool dirty = false;
    const VectorInt2 texture_size = _texture->GetSize(in_param._draw_system);

    for (auto& child : _child_data_array)
    {
        IUIComponent* component = child ? child->_component.get() : nullptr;
        if (nullptr == component)
        {
            continue;
        }

        if (true == component->GetStateFlagBit(UIStateFlag::TResourceDirty))
        {
            if (true == component->UpdateResources(
                *child,
                in_param,
                in_parent_screen_space,
                texture_size
                ))
            {  
                dirty = true;
            }
            component->SetStateFlagBit(UIStateFlag::TResourceDirty, false);
        }
    }

    for (auto& effect : _array_effect_component)
    {
        effect->Update(
            in_param,
            texture_size
            );
    }

    if (true == dirty)
    {
        MarkTextureDirty();
    }

    return dirty;
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
    for(auto& child_data_ptr : _child_data_array)
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;

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
const bool UIHierarchyNode::PreDraw(
    const UIManagerDrawParam& in_draw_param
    )
{
    bool dirty = false;

    // Ensure that children textures are ready
    for (auto& iter : _child_data_array)
    {
        if (nullptr == iter->_component)
        {
            continue;
        }

        if (true == iter->_component->PreDraw(
            in_draw_param,
            *(iter->_node.get())
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

const bool UIHierarchyNode::Draw(
    const UIManagerDrawParam& in_draw_param,
    const bool in_dirty,
    const UIStateFlag in_state_flag
    )
{
    bool dirty = false;
    if ((true == in_dirty) ||
        (false == _texture->GetHasDrawn()) ||
        (true == _texture->GetAlwaysDirty())
        )
    {
        if (false == _texture->SetRenderTarget(
            in_draw_param._draw_system,
            in_draw_param._frame
            ))
        {
            return dirty;
        }

        dirty = true;

        for (auto& iter : _child_data_array)
        {
            UIHierarchyNodeChildData& child_data = *iter;
            if (nullptr == child_data._component)
            {
                continue;
            }

            if (true == child_data._component->GetStateFlagBit(UIStateFlag::THidden))
            {
                continue;
            }

            const auto& shader = in_draw_param._ui_manager->GetShaderRef(UIShaderEnum::TDefault);

            child_data._node->GetUITexture().SetShaderResource(
                *shader,
                0,
                in_draw_param._frame
                );

            if (nullptr == child_data._shader_constant_buffer)
            {
                child_data._shader_constant_buffer = shader->MakeShaderConstantBuffer(
                    in_draw_param._draw_system
                    );
            }

            if (nullptr != child_data._shader_constant_buffer)
            {
                UIManager::TShaderConstantBuffer& buffer = child_data._shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
                buffer._tint_colour = child_data._component->CalculateTintColour();
            }

            in_draw_param._frame->SetShader(shader, child_data._shader_constant_buffer);

            child_data._geometry->Draw(
                in_draw_param._draw_system,
                in_draw_param._frame
                );
        }

        _texture->SetHasDrawn(true);
    }

    if ((true == dirty) &&
        (0 < _array_effect_component.size()))
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

    return dirty;
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
