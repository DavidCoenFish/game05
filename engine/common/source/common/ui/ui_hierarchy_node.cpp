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

/*
UIHierarchyNodeUpdateLayoutParam::UIHierarchyNodeUpdateLayoutParam(
    const std::string& in_locale,
    const float in_ui_scale,
    const float in_time_delta,
    const std::map<std::string, TContentFactory>& in_map_content_factory,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const IUIModel* const in_ui_model,
    LocaleSystem* const in_locale_system,
    TextManager* const in_text_manager
    )
    : _locale(in_locale)
    , _ui_scale(in_ui_scale)
    , _time_delta(in_time_delta)
    , _map_content_factory(in_map_content_factory)
    , _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_model(in_ui_model)
    , _locale_system(in_locale_system)
    , _text_manager(in_text_manager)
{
    // Nop
}
*/

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

UIHierarchyNode::UIHierarchyNode()
    //const LayoutData& in_layout_data,
    //std::unique_ptr<IUIContent>& in_content,
    //std::unique_ptr<UITexture>& in_texture
    //)
    //: _layout_data(in_layout_data)
    //, _content(std::move(in_content))
    //, _texture(std::move(in_texture))
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
    auto geometry = std::make_shared<UIGeometry>(
        in_draw_system,
        in_command_list
        );

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

/*
void UIHierarchyNode::UpdateLayout(
    const IUIData* const in_data,
    const std::string& in_model_key,
    const bool in_draw_to_texture,
    const bool in_always_dirty,
    const VectorInt2& in_parent_size,
    const UIHierarchyNodeUpdateLayoutParam& in_param
    )
{
    bool needs_draw = false;
    auto found = in_param._map_content_factory.find(in_data->GetTemplateName());
    if (found == in_param._map_content_factory.end())
    {
        if (_content)
        {
            needs_draw = true;
        }
        _content.reset();
    }
    else
    {
        UIContentFactoryParam content_factory_param(
            in_param._draw_system,
            in_param._command_list,
            in_param._text_manager
            );
        // Return true for change?
        if (true == (found->second)(_content, content_factory_param))
        {
            needs_draw = true;
        }
    }

    if (nullptr != _content)
    {
        VectorInt2 texture_size;
        // return true for change? example case, child count reduced but no other content change
        if (true == _content->Update(
            texture_size,
            _layout_data,
            _child_data_array,
            in_parent_size,
            in_data,
            in_model_key,
            in_param
            ))
        {
            needs_draw = true;
        }

        VectorFloat4 clear_colour;
        const bool do_clear = _content->GetClearBackground(clear_colour);
        // Texture
        _texture->Update(
            in_param._draw_system,
            in_param._command_list,
            texture_size,
            in_draw_to_texture,
            in_always_dirty,
            do_clear,
            clear_colour
            );

        // update geometry for each child
        for (auto& child_data : _child_data_array)
        {
            VectorFloat4 pos;
            VectorFloat4 uv;

            child_data->_node->GetGeometryData(
                pos,
                uv,
                in_parent_size,
                texture_size
                );
            if (true == child_data->_geometry->Update(
                in_param._draw_system,
                in_param._command_list,
                pos,
                uv
                ))
            {
                needs_draw = true;
            }
        }
    }

    // set texture to dirty on content change
    if ((true == needs_draw) && (_texture))
    {
        _texture->SetHasDrawn(false);
    }

    return;
}
*/

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

/*
const bool UIHierarchyNode::DrawHierarchyRoot(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    Shader* const in_shader,
    const UIManagerDrawData& in_data
    )
{
    const auto parent_size = _texture->GetSize();
    return DrawInternal(
        in_draw_system,
        in_frame,
        in_shader,
        parent_size,
        in_data
        );
}

const bool UIHierarchyNode::DrawHierarchy(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    Shader* const in_shader,
    const VectorInt2& in_new_size,
    const UIManagerDrawData& in_data
    )
{
    return DrawInternal(
        in_draw_system,
        in_frame,
        in_shader,
        in_new_size,
        in_data
        );
}

const bool UIHierarchyNode::DrawInternal(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    Shader* const in_shader,
    const VectorInt2& in_parent_size,
    const UIManagerDrawData& in_data
    )
{
    bool needs_to_draw = false;

    if ((true == _texture->GetAlwaysDirty()) ||
        (false == _texture->GetHasDrawn()) ||
        (true == _content->GetNeedsToDraw()))
    {
        needs_to_draw = true;
    }

    if (true == UpdateChildData(in_draw_system, in_frame))
    {
        needs_to_draw = true;
    }

    if (true == _content->UpdatePosSizeForChildren( // add vectorint2 size for render target to child data?
        in_frame,
        _child_data_array,
        in_parent_size,
        in_data._ui_scale
        ))
    {
        needs_to_draw = true;
    }

    if (true == _content->Update(
        in_draw_system,
        in_frame,
        in_shader,
        in_data,
        _child_data_array
        ))
    {
        needs_to_draw = true;
    }

    if (in_parent_size != _texture->GetSize())
    {
        _texture->SetSize(
            in_draw_system,
            in_frame->GetCommandList(),
            in_parent_size
            );
        needs_to_draw = true;
    }

    if (true == needs_to_draw)
    {
        _content->Draw(
            in_frame,
            _texture.get(),
            _child_data_array,
            in_shader
            );
        _texture->SetHasDrawn(true);
    }

    return needs_to_draw;
}

const bool UIHierarchyNode::UpdateChildData(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    const IUIProviderData* const in_ui_provider_data
    )
{
    //std::string _data_provider_key;
    //int _data_provider_change_id;
    if ((_data_provider_key.empty()) || (nullptr == in_ui_provider_data))
    {
        return false;
    }

    auto data_value = in_ui_provider_data->GetData(_data_provider_key);
    const int change_id = data_value->GetChangeID();
    if (_data_provider_change_id == change_id)
    {
        return false;
    }

    _data_provider_change_id = change_id;
    //std::vector<std::shared_ptr<UIHierarchyNodeChildData>> _child_data_array;

    // we need to rebuild _child_data_array
    in_draw_system;
    in_frame;

    return true;
}
*/
