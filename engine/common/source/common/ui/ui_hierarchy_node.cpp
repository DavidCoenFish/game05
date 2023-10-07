#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/ui/i_ui_input.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_content/ui_content_default.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_string.h"
#include "common/ui/ui_content/ui_content_texture.h"
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
    std::unique_ptr<IUIContent>& in_content,
    std::unique_ptr<UIHierarchyNode>& in_node,
    std::unique_ptr<UIScreenSpace>& in_screen_space
    )
    : _geometry(std::move(in_geometry))
    , _content(std::move(in_content))
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
    LocaleSystem* const in_locale_system,
    TextManager* const in_text_manager,
    const UIDataTextRunStyle* const in_default_text_style 
    )
    : _map_content_factory(in_map_content_factory)
    , _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_model(in_ui_model)
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

//return true if all bits of flga are set
//const bool UIHierarchyNode::GetFlag(const TFlag in_flag)
//{
//    if (((int)_flag & (int)in_flag) == (int)in_flag)
//    {
//        return true;
//    }
//    return false;
//}
//
//void UIHierarchyNode::SetFlag(const TFlag in_flag, const bool in_value)
//{
//    if (true == in_value)
//    {
//        _flag = (TFlag)((int)_flag | (int)in_flag);
//    }
//    else
//    {
//        _flag = (TFlag)((int)_flag & ~(int)in_flag);
//    }
//    return;
//}

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
    std::unique_ptr<IUIContent>& in_content
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
            map_temp_ownership[child->_content->GetSourceToken()] = child;
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
                void* source_child_token = child->_content ? child->_content->GetSourceToken() : nullptr;
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
                std::unique_ptr<IUIContent> content;
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
                UIContentFactoryParam factory_param(
                    in_param._draw_system,
                    in_param._command_list,
                    in_param._text_manager,
                    index
                    );
                if (true == factory->second(
                    child->_content,
                    factory_param
                    ))
                {
                    dirty = true;
                    void* source_token = (void*)&data; //static_cast<void*>(&data); //reinterpret_cast<void*>(&data);
                    child->_content->SetSourceToken(source_token);
                    child->_node->MarkTextureDirty();
                }
            }
            else
            {
                if (nullptr != child->_content)
                {
                    child->_content.reset();
                    dirty = true;
                }
            }

            if (nullptr != child->_content)
            {
                if (true == child->_content->UpdateHierarchy(
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

        if (nullptr == child_data._content)
        {
            continue;
        }

        child_data._content->UpdateSize(
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
            IUIInput* const input = dynamic_cast<IUIInput*>(child_data._content.get());
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
const bool UIHierarchyNode::Draw(
    const UIManagerDrawParam& in_draw_param,
    Shader* const in_shader,
    IUIContent* const in_content_or_null
    )
{
    bool dirty = false;

    // Ensure that children textures are ready
    for (auto& iter : _child_data_array)
    {
        if (true == iter->_node->Draw(
            in_draw_param,
            in_shader,
            iter->_content.get()
            ))
        {
            dirty = true;
        }
    }

    if ((true == dirty) ||
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

        if (nullptr != in_content_or_null)
        {
            in_content_or_null->PreDraw(
                in_draw_param
                //, _child_data_array
                );
        }

        for (auto& iter : _child_data_array)
        {
            UIHierarchyNodeChildData& child_data = *iter;

            in_shader->SetShaderResourceViewHandle(0, child_data._node->GetShaderResourceHeapWrapperItem());
            auto geometry = child_data._geometry->GetGeometry(
                in_draw_param._draw_system,
                in_draw_param._frame->GetCommandList()
                );

            in_draw_param._frame->SetShader(in_shader);
            in_draw_param._frame->Draw(geometry);
        }

        _texture->SetHasDrawn(true);
    }

    return dirty;
}
