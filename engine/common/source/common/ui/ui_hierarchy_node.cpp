#include "common/common_pch.h"
#include "common/ui/ui_hierarchy_node.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_content_canvas.h"
#include "common/ui/ui_content_stack.h"
#include "common/ui/ui_content_text.h"
#include "common/ui/ui_content_texture.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/i_ui_data.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"

UIHierarchyNodeUpdateLayoutParam::UIHierarchyNodeUpdateLayoutParam(
    const std::string& in_locale,
    const float in_ui_scale,
    const float in_time_delta,
    const std::map<std::string, TContentFactory>& in_map_content_factory,
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const IUIModel* const in_ui_model,
    LocaleSystem* const in_locale_system
    )
    : _locale(in_locale)
    , _ui_scale(in_ui_scale)
    , _time_delta(in_time_delta)
    , _map_content_factory(in_map_content_factory)
    , _draw_system(in_draw_system)
    , _command_list(in_command_list)
    , _ui_model(in_ui_model)
    , _locale_system(in_locale_system)
{
    // Nop
}

UIHierarchyNode::LayoutData UIHierarchyNode::LayoutData::FactoryFull()
{
    return LayoutData(
        UICoord(1.0f, 0.0f),
        UICoord(1.0f, 0.0f),
        UICoord(0.0f, 0.0f),
        UICoord(0.0f, 0.0f),
        UICoord(0.0f, 0.0f),
        UICoord(0.0f, 0.0f)
        );
}

UIHierarchyNode::LayoutData::LayoutData(
    const UICoord& in_size_x,
    const UICoord& in_size_y,
    const UICoord& in_attach_x,
    const UICoord& in_attach_y,
    const UICoord& in_pivot_x,
    const UICoord& in_pivot_y
    )
{
    _data_size[0] = in_size_x;
    _data_size[1] = in_size_y;
    _data_attach[0] = in_attach_x;
    _data_attach[1] = in_attach_y;
    _data_pivot[0] = in_pivot_x;
    _data_pivot[0] = in_pivot_y;
}

UIHierarchyNodeChildData::UIHierarchyNodeChildData(
    const std::shared_ptr<UIHierarchyNode>& in_node,
    const std::shared_ptr<UIGeometry>& in_geometry//,
    //const VectorInt2& in_render_target_size
    )
    : _node(in_node)
    , _geometry(in_geometry)
    //, _render_target_size(in_render_target_size)
{
    // Nop
}

/*
std::shared_ptr<UITexture> UIHierarchyNode::MakeTextureRenderTarget(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list,
    const VectorFloat4& in_clear_colour,
    const bool in_allow_clear,
    const bool in_always_dirty
    )
{
    return std::make_shared<UITexture>(
        in_draw_system,
        in_command_list,
        false,
        in_always_dirty,
        in_allow_clear,
        in_clear_colour
        );
}

VectorFloat4 UIHierarchyNode::CalculatePos(
    const VectorInt2& in_parent_size,
    const LayoutData& in_layout_data,
    const float in_ui_scale
    )
{
    const float x = (float)in_parent_size.GetX();
    const float y = (float)in_parent_size.GetY();

    //UICoord _data_size[2];
    const float sizeX = in_layout_data._data_size[0].Calculate(x, in_ui_scale);
    const float sizeY = in_layout_data._data_size[1].Calculate(y, in_ui_scale);

    //UICoord _data_attach[2];
    const float attachX = in_layout_data._data_attach[0].Calculate(x, in_ui_scale);
    const float attachY = in_layout_data._data_attach[1].Calculate(y, in_ui_scale);

    //UICoord _data_pivot[2];
    const float pivotX = in_layout_data._data_pivot[0].Calculate(sizeX, in_ui_scale);
    const float pivotY = in_layout_data._data_pivot[1].Calculate(sizeY, in_ui_scale);

    const float childX = attachX - pivotX;
    const float childY = attachY - pivotY;

    return VectorFloat4(
        (childX / x) * 2.0f - 1.0f,
        (childY / y) * 2.0f - 1.0f,
        ((childX + sizeX) / x) * 2.0f - 1.0f,
        ((childY + sizeY) / y) * 2.0f - 1.0f
        );
}

VectorInt2 UIHierarchyNode::CalculateSizeInt(
    const VectorInt2& in_parent_size,
    const LayoutData& in_layout_data,
    const float in_ui_scale
    )
{
    const float x = (float)in_parent_size.GetX();
    const float y = (float)in_parent_size.GetY();

    //UICoord _data_size[2];
    const int sizeX = (int)ceil(in_layout_data._data_size[0].Calculate(x, in_ui_scale));
    const int sizeY = (int)ceil(in_layout_data._data_size[1].Calculate(y, in_ui_scale));

    return VectorInt2(
        sizeX,
        sizeY
        );
}
*/

UIHierarchyNode::UIHierarchyNode(
    const LayoutData& in_layout_data,
    const std::shared_ptr<IUIContent>& in_content,
    const std::shared_ptr<UITexture>& in_texture,
    const TFlag in_flag
    )
    : _layout_data(in_layout_data)
    , _content(in_content)
    , _texture(in_texture)
    , _flag(in_flag)
{
    // Nop
}

UIHierarchyNode::~UIHierarchyNode()
{
    // Nop
}


//LayoutData& UIHierarchyNode::GetLayoutDataModify()
//{
//    
//}

const VectorInt2 UIHierarchyNode::GetTargetSize() const
{
    return _texture->GetSize();
}

//return true if all bits of flga are set
const bool UIHierarchyNode::GetFlag(const TFlag in_flag)
{
    if (((int)_flag & (int)in_flag) == (int)in_flag)
    {
        return true;
    }
    return false;
}

void UIHierarchyNode::SetFlag(const TFlag in_flag, const bool in_value)
{
    if (true == in_value)
    {
        _flag = (TFlag)((int)_flag | (int)in_flag);
    }
    else
    {
        _flag = (TFlag)((int)_flag & ~(int)in_flag);
    }
    return;
}

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
