#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_string.h"
#include "common/ui/ui_content/ui_content_texture.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/i_ui_data.h"
#include "common/ui/ui_root_input_state.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

UIHierarchyNodeChildData::UIHierarchyNodeChildData(
    std::unique_ptr<UIGeometry>& in_geometry,
    std::unique_ptr<IUIContent>& in_content,
    std::unique_ptr<UIHierarchyNode>& in_node,
    const VectorFloat4& in_input_screen_size
    )
    : _geometry(std::move(in_geometry))
    , _content(std::move(in_content))
    , _node(std::move(in_node))
    , _input_screen_size(in_input_screen_size)
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
    TextManager* const in_text_manager
    )
    : _map_content_factory(in_map_content_factory)
    , _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_model(in_ui_model)
    , _locale_system(in_locale_system)
    , _text_manager(in_text_manager)
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
    // Nop
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
    auto child = std::make_shared<UIHierarchyNodeChildData>(
        geometry,
        in_content,
        node
        );
    _child_data_array.push_back(child);
}

void UIHierarchyNode::ClearChildren()
{
    _child_data_array.clear();
}

const bool UIHierarchyNode::UpdateHierarchy(
    const UIHierarchyNodeUpdateHierarchyParam& in_param,
    const std::vector<std::shared_ptr<IUIData>>* const in_array_data_or_null,
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
            map_temp_ownership[child->_content->GetSourceUIDataToken()] = child;
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
                void* source_child_token = child->_content->GetSourceUIDataToken();
                void* data_token = (void*)&data;
                if (data_token != source_child_token)
                {
                    map_temp_ownership[source_child_token] = child;
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
                _child_data_array[index] = std::make_shared<UIHierarchyNodeChildData>(
                    geometry,
                    content, 
                    node
                    );
                dirty = true;
            }
        }

        // ensure each content is created/ passed through factory
        for (int index = 0; index < target_length; ++index)
        {
            const auto& data = (in_array_data_or_null->operator[](index));
            auto& child = _child_data_array[index];

            auto factory = in_param._map_content_factory.find(data->GetTemplateName());
            if (factory != in_param._map_content_factory.end())
            {
                UIContentFactoryParam factory_param(
                    in_param._draw_system,
                    in_param._command_list,
                    in_param._text_manager
                    );
                if (true == factory->second(
                    child->_content,
                    factory_param
                    ))
                {
                    dirty = true;
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
                    data.get(),
                    *(child.get()),
                    in_param
                    ))
                {
                    dirty = true;
                }
            }
        }
    }

    return dirty;
}

//void UIHierarchyNode::UpdateDesiredSize(
//    const VectorInt2& in_parent_desired_size,
//    const UIHierarchyNodeUpdateDesiredSize& in_param
//    )
//{
//}
//
//void UIHierarchyNode::UpdateSize(
//    const VectorInt2& in_parent_size,
//    const VectorFloat4& in_input_screen_parent,
//    const UIHierarchyNodeUpdateSize& in_param
//    )
//{
//}

//UIManager::Draw(node N0)
//    N0::Draw(shader)
//        for each of A0
//            N1::Draw(shader)
//        set T0 as render target
//        for each of A0
//            draw G1 with texture T1

// Return True if we needed to draw, ie, the texture may have changed
const bool UIHierarchyNode::Draw(
    const UIManagerDrawParam& in_draw_param,
    Shader* const in_shader
    )
{
    bool dirty = false;
    // Ensure that child texture is ready
    for (auto& iter : _child_data_array)
    {
        if (true == iter->_node->Draw(
            in_draw_param,
            in_shader
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
        dirty = true;

        auto* render_target = _texture->GetRenderTarget(
            in_draw_param._draw_system,
            in_draw_param._frame->GetCommandList()
            );
        in_draw_param._frame->SetRenderTarget(
            render_target, 
            _texture->GetAllowClear()
            );

        for (auto& iter : _child_data_array)
        {
            UIHierarchyNodeChildData& child_data = *iter;
            child_data._content->Draw(
                in_draw_param,
                in_shader,
                child_data._geometry.get(),
                child_data._node->GetShaderResourceHeapWrapperItem()
                );
        }

        _texture->SetHasDrawn(true);
    }

    return dirty;
}
