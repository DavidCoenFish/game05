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

UIHierarchyNodeLayoutData UIHierarchyNodeLayoutData::FactoryFull()
{
    return UIHierarchyNodeLayoutData(
        UICoord(UICoord::ParentSource::X, 1.0f, 0.0f),
        UICoord(UICoord::ParentSource::Y, 1.0f, 0.0f),
        UICoord(UICoord::ParentSource::X, 0.0f, 0.0f),
        UICoord(UICoord::ParentSource::Y, 0.0f, 0.0f),
        UICoord(UICoord::ParentSource::X, 0.0f, 0.0f),
        UICoord(UICoord::ParentSource::Y, 0.0f, 0.0f)
        );
}

UIHierarchyNodeLayoutData::UIHierarchyNodeLayoutData(
    const UICoord& in_size_x,
    const UICoord& in_size_y,
    const UICoord& in_attach_x,
    const UICoord& in_attach_y,
    const UICoord& in_pivot_x,
    const UICoord& in_pivot_y,
    const VectorFloat2& in_uv_scroll
    )
    : _uv_scroll(in_uv_scroll)
{
    _data_size[0] = in_size_x;
    _data_size[1] = in_size_y;
    _data_attach[0] = in_attach_x;
    _data_attach[1] = in_attach_y;
    _data_pivot[0] = in_pivot_x;
    _data_pivot[1] = in_pivot_y;
    return;
}

UIHierarchyNodeChildData::UIHierarchyNodeChildData(
    const std::shared_ptr<UIHierarchyNode>& in_node,
    const std::shared_ptr<UIGeometry>& in_geometry
    )
    : _node(in_node)
    , _geometry(in_geometry)
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

const VectorInt2 UIHierarchyNode::GetTargetSize() const
{
    return _texture->GetSize();
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
}

void UIHierarchyNode::UpdateSize(
    const VectorInt2& in_parent_size,
    const VectorFloat4& in_input_screen_parent,
    const UIHierarchyNodeUpdateSize& in_param
    )
{
}

// Return True if we needed to draw, ie, the texture may have changed
const bool UIHierarchyNode::Draw(
    const UIManagerDrawParam& in_draw_param,
    Shader* const in_shader
    )
{
    bool needs_to_draw = false;
    for (auto& iter : _child_data_array)
    {
        if (true == iter->_node->Draw(
            in_draw_param,
            in_shader
            ))
        {
            needs_to_draw = true;
        }
    }

    if ((true == needs_to_draw) ||
        (false == _texture->GetHasDrawn()) ||
        (true == _texture->GetAlwaysDirty()) ||
        (true == _content->GetNeedsDraw())
        )
    {
        needs_to_draw = true;

        _content->Draw(
            in_draw_param,
            _texture.get(),
            _child_data_array,
            in_shader
            );

        _texture->SetHasDrawn(true);

    }

    return needs_to_draw;
}
