#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/ui/i_ui_input.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_component/ui_component_default.h"
#include "common/ui/ui_component/ui_component_stack.h"
#include "common/ui/ui_component/ui_component_string.h"
#include "common/ui/ui_component/ui_component_texture.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_screen_space.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_root_input_state.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

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

UIHierarchyNodeChildData::~UIHierarchyNodeChildData()
{
    // Nop
}

UIHierarchyNodeUpdateHierarchyParam::UIHierarchyNodeUpdateHierarchyParam(
    const std::map<std::string, TContentFactory>& in_map_content_factory,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const IUIModel* const in_ui_model,
    UIManager* const in_ui_manager,
    LocaleSystem* const in_locale_system,
    TextManager* const in_text_manager,
    const UIDataTextRunStyle* const in_default_text_style 
    )
    : _map_content_factory(in_map_content_factory)
    , _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_model(in_ui_model)
    , _ui_manager(in_ui_manager)
    , _locale_system(in_locale_system)
    , _text_manager(in_text_manager)
    , _default_text_style(in_default_text_style)
{
    // Nop
}

UIHierarchyNodeUpdateDesiredSize::UIHierarchyNodeUpdateDesiredSize(
    const float in_ui_scale,
    const float in_time_delta
    )
    : _ui_scale(in_ui_scale)
    , _time_delta(in_time_delta)
{
    // Nop
};

UIHierarchyNodeUpdateSize::UIHierarchyNodeUpdateSize()
{
    // Nop
};

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

std::shared_ptr<HeapWrapperItem> UIHierarchyNode::GetShaderResourceHeapWrapperItem() const
{
    return _texture->GetShaderResourceHeapWrapperItem();
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

const bool UIHierarchyNode::UpdateHierarchy(
    const UIHierarchyNodeUpdateHierarchyParam& in_param,
    const std::vector<std::shared_ptr<UIData>>* const in_array_data_or_null,
    const bool in_draw_to_texture,
    const bool in_always_dirty,
    const bool in_allow_clear,
    const VectorFloat4& in_clear_colour
    )
{
    bool dirty = false;

    // Update texture
    if (nullptr == _texture)
    {
        _texture = std::make_unique<UITexture>(
            in_draw_to_texture,
            in_always_dirty,
            in_allow_clear,
            in_clear_colour
            );
        dirty = true;
    }
    else
    {
        if (true == _texture->Update(
            in_draw_to_texture,
            in_always_dirty,
            in_allow_clear,
            in_clear_colour
            ))
        {
            dirty = true;
        }
    }

    // Update child data array
    if (nullptr == in_array_data_or_null)
    {
        if (0 != _child_data_array.size())
        {
            dirty = true;
        }
        _child_data_array.clear();
    }
    else
    {
        std::map<void*, std::shared_ptr<UIHierarchyNodeChildData>> map_temp_ownership;

        //std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;
        const int target_length = (int)in_array_data_or_null->size();
        for (int index = target_length; index < (int)_child_data_array.size(); ++index)
        {
            auto child = _child_data_array[index];
            map_temp_ownership[child->_component->GetSourceToken()] = child;
            dirty = true;
        }
        _child_data_array.resize(target_length);

        // Remove mismatched items in child data array
        for (int index = 0; index < target_length; ++index)
        {
            const auto& data = *(in_array_data_or_null->operator[](index));
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
            const auto& data = *(in_array_data_or_null->operator[](index));
            void* data_token = (void*)&data;
            auto found = map_temp_ownership.find(data_token);
            if (found != map_temp_ownership.end())
            {
                _child_data_array[index] = found->second;
            }
            else
            {
                auto geometry = std::make_unique<UIGeometry>();
                std::unique_ptr<IUIComponent> content;
                auto node = std::make_unique<UIHierarchyNode>();
                auto screen_space = std::make_unique<UIScreenSpace>();
                _child_data_array[index] = std::make_shared<UIHierarchyNodeChildData>(
                    geometry,
                    content, 
                    node,
                    screen_space
                    );
                dirty = true;
            }
        }

        // ensure each content is created/ passed through factory
        for (int index = 0; index < target_length; ++index)
        {
            auto& data = *(in_array_data_or_null->operator[](index));
            auto& child = _child_data_array[index];

            auto factory = in_param._map_content_factory.find(data.GetTemplateName());
            if (factory != in_param._map_content_factory.end())
            {
                UIComponentFactoryParam factory_param(
                    in_param._draw_system,
                    in_param._command_list,
                    in_param._text_manager,
                    index
                    );
                if (true == factory->second(
                    child->_component,
                    factory_param
                    ))
                {
                    dirty = true;
                    void* source_token = (void*)&data; //static_cast<void*>(&data); //reinterpret_cast<void*>(&data);
                    child->_component->SetSourceToken(source_token);
                    child->_node->MarkTextureDirty();
                }
            }
            else
            {
                if (nullptr != child->_component)
                {
                    child->_component.reset();
                    dirty = true;
                }
            }

            if (nullptr != child->_component)
            {
                if (true == child->_component->UpdateHierarchy(
                    &data,
                    *(child.get()),
                    in_param
                    ))
                {
                    dirty = true;
                    child->_node->MarkTextureDirty();
                }
            }
        }
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

void UIHierarchyNode::UpdateTextureSize(
    const VectorInt2& in_parent_size,
    const bool in_mark_dirty
    )
{
    _texture->SetSize(in_parent_size);
    if (true == in_mark_dirty)
    {
        _texture->MarkDirty();
    }
}

// todo: UpdateSize doesn't need to return, _texture marks self as needs to draw on size change
void UIHierarchyNode::UpdateSize(
    DrawSystem* const in_draw_system,
    const VectorInt2& in_parent_size,
    const VectorInt2& in_parent_offset,
    const VectorInt2& in_parent_window,
    const float in_ui_scale,
    const float in_time_delta,
    const bool in_mark_dirty,
    const UIScreenSpace& in_parent_screen_space
    )
{
    UpdateTextureSize(in_parent_size, in_mark_dirty);

    // for each child, work out the geometry and texture size for each content
    for (auto& child_data_ptr : _child_data_array)
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;

        if (nullptr == child_data._component)
        {
            continue;
        }

        child_data._component->UpdateSize(
            in_draw_system,
            in_parent_size,
            in_parent_offset,
            in_parent_window,
            in_ui_scale,
            in_time_delta,
            *(child_data._geometry.get()),
            *(child_data._node.get()),
            in_parent_screen_space,
            *(child_data._screen_space.get())
            );
    }

    return;
}

void UIHierarchyNode::DealInput(
    UIRootInputState& in_input_state,
    const bool in_mouse_inside
    )
{
    for(auto& child_data_ptr : _child_data_array)
    {
        UIHierarchyNodeChildData& child_data = *child_data_ptr;
        const bool inside = in_mouse_inside && child_data._screen_space->GetClipRef().Inside(in_input_state.GetMousePosRef());
        if ((true == inside) &&        
            (false == in_input_state.GetMouseLeftDown()) &&
            (true == in_input_state.GetMouseLeftDownChange()))
        {
            IUIInput* const input = dynamic_cast<IUIInput*>(child_data._component.get());
            if (nullptr != input)
            {
                input->OnInputMouseClick(
                    child_data._screen_space->GetPosRef(),
                    in_input_state.GetMousePosRef()
                    );
            }
        }

        child_data_ptr->_node->DealInput(
            in_input_state,
            inside
            );
    }
    return;
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
        if (true == iter->_component->Draw(
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
    const bool in_dirty
    )
{
    bool dirty = false;
    if ((true == in_dirty) ||
        (false == _texture->GetHasDrawn()) ||
        (true == _texture->GetAlwaysDirty())
        )
    {
        std::shared_ptr<IResource> frame_resource;
        auto* const render_target = _texture->GetRenderTarget(
            in_draw_param._draw_system,
            frame_resource,
            in_draw_param._frame->GetCommandList()
            );
        if (nullptr == render_target)
        {
            return dirty;
        }

        dirty = true;
        in_draw_param._frame->SetRenderTarget(
            render_target, 
            frame_resource,
            _texture->GetAllowClear()
            );

        for (auto& iter : _child_data_array)
        {
            UIHierarchyNodeChildData& child_data = *iter;

            Shader* const shader = in_draw_param._ui_manager->GetDefaultShader();

            shader->SetShaderResourceViewHandle(0, child_data._node->GetShaderResourceHeapWrapperItem());
            auto geometry = child_data._geometry->GetGeometry(
                in_draw_param._draw_system,
                in_draw_param._frame->GetCommandList()
                );
            if (nullptr == child_data._shader_constant_buffer)
            {
                child_data._shader_constant_buffer = shader->MakeShaderConstantBuffer(
                    in_draw_param._draw_system
                    );
            }
            ShaderConstantBuffer* const shader_constant_buffer = child_data._shader_constant_buffer.get();
            if (nullptr != shader_constant_buffer)
            {
                UIManager::TShaderConstantBuffer& buffer = shader_constant_buffer->GetConstant<UIManager::TShaderConstantBuffer>(0);
                // todo: get the tint colour, from geometry?
                buffer._tint_colour = VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f);
            }

            in_draw_param._frame->SetShader(shader, shader_constant_buffer);
            in_draw_param._frame->Draw(geometry);
        }

        _texture->SetHasDrawn(true);
    }

    return dirty;
}
