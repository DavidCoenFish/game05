#include "common/common_pch.h"
#include "common/ui/i_ui_content.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_texture.h"

IUIContent::IUIContent()
{
    // Nop
}

IUIContent::~IUIContent()
{
    // Nop
}

const bool IUIContent::GetClearBackground(
    VectorFloat4&
    ) const
{
    return false;
}

const bool IUIContent::GetNeedsDraw() const
{
    return false;
}

/*
const bool IUIContent::UpdatePosSizeForChildren( // add vectorint2 size for render target to child data?
    DrawSystemFrame* const in_frame,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
    const VectorInt2& in_parent_size,
    const float in_ui_scale
    )
{
    bool result = false;
    for (auto& child_data : in_child_data_array)
    {
        const auto child_pos = UIHierarchyNode::CalculatePos(
            in_parent_size,
            child_data->_node->GetLayoutData(),
            in_ui_scale
        );
        if (true == child_data->_geometry->Update(
            in_frame,
            child_pos,
            VectorFloat4(0.0f, 1.0f, 1.0f, 0.0f)
        ))
        {
            result = true;
        }
        child_data->_render_target_size = UIHierarchyNode::CalculateSizeInt(
            in_parent_size,
            child_data->_node->GetLayoutData(),
            in_ui_scale
        );
    }
    return result;
}


const bool IUIContent::Update(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_frame,
    Shader* const in_shader,
    const UIManagerDrawData& in_data,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array
    )
{
    bool result = false;
    for (auto& child_data : in_child_data_array)
    {
        if (true == child_data->_node->DrawHierarchy(
            in_draw_system,
            in_frame,
            in_shader,
            child_data->_render_target_size,
            in_data
            ))
        {
            result = true;
        }
    }
    return result;
}
*/

void IUIContent::Draw(
    DrawSystemFrame* const in_frame,
    UITexture* const in_texture,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
    Shader* const in_shader
    )
{
    in_frame->SetRenderTarget(
        in_texture->GetRenderTarget(), 
        in_texture->GetAllowClear()
        );
    for (auto& child_data : in_child_data_array)
    {
        in_shader->SetShaderResourceViewHandle(
            0, 
            child_data->_node->GetShaderResourceHeapWrapperItem()
            );
        in_frame->SetShader(in_shader);
        in_frame->Draw(child_data->_geometry->GetGeometry());
    }

    return;
}
