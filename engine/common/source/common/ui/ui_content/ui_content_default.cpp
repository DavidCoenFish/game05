#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_default.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_manager.h"

#include "common/common_pch.h"
#include "common/ui/ui_content/ui_content_stack.h"
#include "common/ui/ui_content/ui_content_default.h"
#include "common/ui/ui_layout.h"
#include "common/ui/ui_coord.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_hierarchy_node.h"

UIContentDefault::UIContentDefault(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout,
    void* in_source_token
    )
    : _clear_background(in_clear_background)
    , _clear_colour(in_clear_colour)
    , _layout(in_layout)
    , _source_token(in_source_token)
{
    // Nop
}

UIContentDefault::~UIContentDefault()
{
    // Nop
}

const bool UIContentDefault::SetBase(
    const bool in_clear_background,
    const VectorFloat4& in_clear_colour,
    const UILayout& in_layout
    )
{
    bool dirty = false;

    if (_clear_background != in_clear_background)
    {
        dirty = true;
        _clear_background = in_clear_background;
    }
    if (_clear_colour != in_clear_colour)
    {
        //change of background colour when background is not active doesn't dirty content
        if (true == _clear_background)
        {
            dirty = true;
        }
        _clear_colour = in_clear_colour;
    }


    if (_layout != in_layout)
    {
        dirty = true;
        _layout = in_layout;
    }

    return dirty;
}

//const bool UIContentDefault::GetClearBackground(
//    VectorFloat4& out_clear_colour
//    ) const
//{
//    if (_clear_background)
//    {
//        out_clear_colour = _clear_colour;
//    }
//    return _clear_background;
//}

// Make sorting children easier
void UIContentDefault::SetSourceToken(void* in_source_ui_data_token)
{
    _source_token = in_source_ui_data_token;
}

void* UIContentDefault::GetSourceToken() const
{
    return _source_token;
}

const bool UIContentDefault::SetLayout(const UILayout& in_layout)
{
    bool dirty = false;
    if (_layout != in_layout)
    {
        _layout = in_layout;
    }
    return dirty;
}

const bool UIContentDefault::UpdateHierarchy(
    //std::vector<std::shared_ptr<IUIData>>*& out_array_data_or_null,
    const IUIData* const in_data,
    UIHierarchyNodeChildData& in_out_child_data,
    const UIHierarchyNodeUpdateHierarchyParam& in_param
    )
{
    bool dirty = false;
    const UIDataContainer* const data_container = dynamic_cast<const UIDataContainer*>(in_data);
    if (nullptr != data_container)
    {
        //const std::vector<std::shared_ptr<IUIData>>& array_data = data_container->GetDataConst();
        data_container->VisitDataArray([
            &in_out_child_data,
            &in_param,
            this
            ](const std::vector<std::shared_ptr<IUIData>>& in_array_data){
            in_out_child_data._node->UpdateHierarchy(
                in_param,
                &in_array_data,
                true,
                false,
                _clear_background,
                _clear_colour
                );
        });
    }
    else
    {
        if (true == in_out_child_data._node->UpdateHierarchy(
            in_param,
            nullptr,
            true,
            false,
            _clear_background,
            _clear_colour
            ))
        {
            dirty = true;
        }
    }

    return dirty;
}

void UIContentDefault::UpdateSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale, 
    UIGeometry& in_out_geometry, 
    UIHierarchyNode& in_out_node // ::GetDesiredSize may not be const, allow cache pre vertex data for text
    )
{
    DSC_TODO("unimplemented");
}

const VectorInt2 UIContentDefault::GetDesiredSize(
    const VectorInt2& in_parent_size,
    const float in_ui_scale
    )
{
    DSC_TODO("unimplemented");
    return in_parent_size;
}

void UIContentDefault::Draw(
    const UIManagerDrawParam& in_param,
    Shader* const in_shader,
    UIGeometry* const in_geometry,
    const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item // The texture resource of the node for this content
    )
{
    in_shader->SetShaderResourceViewHandle(0, in_heap_wrapper_item);
    auto geometry = in_geometry->GetGeometry(
        in_param._draw_system,
        in_param._frame->GetCommandList()
        );

    in_param._frame->SetShader(in_shader);
    in_param._frame->Draw(geometry);

    return;
}
