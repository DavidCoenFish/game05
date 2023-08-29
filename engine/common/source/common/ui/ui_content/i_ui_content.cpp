#include "common/common_pch.h"
#include "common/ui/ui_content/i_ui_content.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/shader/shader.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float4.h"
#include "common/ui/ui_geometry.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_manager.h"

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

void IUIContent::SetDrawn()
{
    // Nop
}

const bool IUIContent::Update(
    VectorInt2&,
    UIHierarchyNodeLayoutData&,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>&,
    const VectorInt2&,
    const IUIData* const,
    const std::string&,
    const UIHierarchyNodeUpdateLayoutParam&
    )
{
    return false;
}


// Default implementation, draw each child's texture on the child's geometry to our node's texture
void IUIContent::Draw(
    const UIManagerDrawParam& in_param,
    UITexture* const in_texture,
    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
    Shader* const in_shader
    )
{
    in_param._frame->SetRenderTarget(
        in_texture->GetRenderTarget(), 
        in_texture->GetAllowClear()
        );

    for (auto& child_data : in_child_data_array)
    {
        in_shader->SetShaderResourceViewHandle(
            0, 
            child_data->_node->GetShaderResourceHeapWrapperItem()
            );
        in_param._frame->SetShader(in_shader);
        in_param._frame->Draw(child_data->_geometry->GetGeometry());
    }

    SetDrawn();

    return;
}
