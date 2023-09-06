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
    : _geometry(in_geometry)
    , _content(in_content)
    , _node(in_node)
    , _input_screen_size(in_input_screen_size)
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
    const std::shared_ptr<UIHierarchyNode>& in_node,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    auto geometry = std::make_shared<UIGeometry>();
    auto child = std::make_shared<UIHierarchyNodeChildData>(
        in_node,
        geometry
        );
    _child_data_array.push_back(child);
}

void UIHierarchyNode::ClearChildren()
{
    _child_data_array.clear();
}

const bool UIHierarchyNode::UpdateHierarchy(
    const UIHierarchyNodeUpdateHierarchyParam& in_param,
    const IUIData& in_data,
    const bool in_draw_to_texture,
    const bool in_always_dirty
    )
{
    bool dirty = false;
    auto factory = in_param._map_content_factory.find(in_data.GetTemplateName());
    if (factory != in_param._map_content_factory.end())
    {
        UIContentFactoryParam factory_param(
            in_param._draw_system,
            in_param._command_list,
            in_param._text_manager
            );
        if (true == factory->second(
            _content,
            factory_param
            ))
        {
            dirty = true;
        }
    }
    else
    {
        if (_content)
        {
            _content.reset();
            dirty = true;
        }
        return dirty;
    }

    if (true == _content->UpdateHierarchy(
        _child_data_array,
        in_data,
        in_param
        ))
    {
        dirty = true;
    }

    VectorFloat4 clear_colour;
    const bool allow_clear = _content->GetClearBackground(clear_colour);

    if (nullptr == _texture)
    {
        _texture = std::make_unique<UITexture>(
            in_param._draw_system,
            in_draw_to_texture,
            in_always_dirty,
            allow_clear,
            clear_colour
            );
        dirty = true;
    }
    else
    {
        if (true == _texture->Update(
            in_draw_to_texture,
            in_always_dirty,
            allow_clear,
            clear_colour
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

void UIHierarchyNode::UpdateDesiredSize(
    const VectorInt2& in_parent_desired_size,
    const UIHierarchyNodeUpdateDesiredSize& in_param
    )
{
    _content->UpdateDesiredSize(
        _child_data_array,
        *(_layout.get()),
        in_parent_desired_size, 
        in_param
        );
}

void UIHierarchyNode::UpdateSize(
    const VectorInt2& in_parent_size,
    const VectorFloat4& in_input_screen_parent,
    const UIHierarchyNodeUpdateSize& in_param
    )
{
    _content->UpdateSize(
        _child_data_array,
        *(_layout.get()),
        in_parent_size, 
        in_input_screen_parent,
        in_param
        );

}

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
